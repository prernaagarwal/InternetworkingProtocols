//Amanda Williams and Prerna Agarwal
// CS494, Project Part 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "packets.h"
#include <math.h>
#include <signal.h>
#include <time.h>
#include<pthread.h>
using namespace std;

//prototypes
void timer_thread(union sigval arg);
void error_msg(const char * message);
void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size, sockaddr_in client_socket, socklen_t clilen);
void *sender(void * args);
void *receiver(void * args);

//pthread things
timer_t timer;
struct sigevent sigevt;
struct itimerspec timerspec;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_t sendthread;
pthread_t recvthread;


int globalsock;
struct sockaddr_in globalclient;
socklen_t globalclisize;
void * globalptr;

//GLOBALS FOR PROJECT 2 
//currentbase and shiftedby are part of critical section, both threads will update them.
int N; //window size 
int retransmitted; //number of retransmitted packets
int totalpacketssent; //number of total packets sent, including retransmissions
int currentbase; //the curent base packet number (for the window)
int shiftedby; //number of elements the window has shifted.
FILE * gfile;//filename
int gfilesize;//for the file size
double total_time; //store total time taken for file transfer
int acknum = 4;// global ack number

packet * array;//array of packets, also critical section. Dynamically allocated in main.

int main(int argc, char * argv[])
{
	int sock, newsock, portnum, ret; //file descriptors, port number and variable to caputer return values
	void * buffer;			 // buffer
	struct sockaddr_in serv_addr;	 //server address
	struct sockaddr_in client_addr;	 //client address
	socklen_t clisize;		 //client size
	char *filename;			 //file name
	int filesize = 0;		 //filesize
	
	cout<<"In server"<<endl;

	//parameters passed in from command line
	//server and portnumber
	if(argc < 2)
	{
		printf("No Port Number Provided");
		exit(1);
	}
        //check if we were given a window size
        if(argc < 3)
        {
            cout<<"No window size provided." <<endl;
            exit(1);
        }

	// set serv_addr to 0	
	memset(&serv_addr, 0, sizeof(serv_addr));
	
	//set the port num to what was passed in. String to int.
	portnum = atoi(argv[1]);	


        N = atoi(argv[2]);//set the window size to the argument passed in
        array = new packet[N];//dynamically allocate the array of packets. Has to be dynamic as the user
        //passes in a size at runtime. 
        shiftedby = N;//setting shifted by to N here so that when we first start
        //the sending thread, it will read N items into the array. 


	//create socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_msg("Cannot Open Socket");
	else
		printf("Socket now open \n");


	//set the servers information
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portnum);
	
	globalsock = sock;

	//bind socket with server
	if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error_msg("Error, could not bind.");
		exit(1);
	}
	else
		printf("We are now bound!\n");

	//get client size and allocate memory for buffer
	clisize = sizeof(client_addr);
	buffer = malloc(PTR_SIZE);//creating buffer for maximum packet length

	// get SYN and filesize request from the client
	ret = recvfrom(sock, buffer, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
	if(ret < 0)
		error_msg("Recieve Failed");
	else
		cout<<"Recieved"<<endl;
	globalclient = client_addr;
	globalclisize = clisize;
		
	//check if we have the file in question

	
	packet filerequest;
	filerequest.deserialize(buffer);
	char * filerequested = (char *)filerequest.data;
	cout<<"file requested to char: "<<filerequested<<endl;
	if(filerequest.type == SYN)
	{
		cout<<"SYN PACKET RECIEVED"<<endl;	
	}
	else
		error_msg("NOT A SYN PACKET.");


	//check for that file
	if(access(filerequested, F_OK) == -1)
	{
		error_msg("We do not have that file.");
	}
	else{
		cout<<"File has been found!" <<endl;
	}
	
	// open the file	
	FILE * file = fopen(filerequested, "r");
        gfile = file;//point to file too.
	if(file == NULL)
		cout<<"File Open is NULL"<<endl;	


	//Gets the size of the file we want
	struct stat file_stat;
	stat(filerequested, &file_stat);
	filesize = file_stat.st_size;
	cout<<"file size = " <<filesize<<endl;
        gfilesize = filesize;


	//create pointer to point to the file size so that we may pass this back to the client in our SYN_ACK
	void * data = &filesize; 
	send_data_packet(sock, SYN_ACK, 2, data, PCKLEN, client_addr, clisize);//sequence number of 2 since we've already recv'd reguest
			
	//recv the ack from client before transmitting file.
	void * ack_to_begin = malloc(PTR_SIZE);
	packet beginpacket;

	//receiving the confirmation from client to start sending file	
	ret = recvfrom(sock, ack_to_begin, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
	if(ret < 0)
		error_msg("ack to begin file transfer failed");
	else
		cout<<"Recieved"<<endl;
	beginpacket.deserialize(ack_to_begin);
	free(ack_to_begin);

	if (beginpacket.type == ACK)
		cout<<"ACK to transfer file received\n";
	
/*	
	//total bytes we have sent to client. will let us know when to close the connection
	int totalbytes =0;
	float total_packets_to_send = ceil(filesize /1024.0);//the number of packets we will be sending rounded up
	cout<<"total packets to be sent" <<total_packets_to_send<<endl;
			
	//to loop while we transfer the files. 
	void * readData = malloc(PCKLEN);
	int seq_num = 4; // sequence number of the packet after initial syn, syn_ack, ack
	int i =0;
	int sequence_we_got = 0;

	int thread_param = 31415; //Parameter to pass

	//Populate sigevt structure
	sigevt.sigev_notify = SIGEV_THREAD;
	sigevt.sigev_value.sival_ptr = &thread_param;
	sigevt.sigev_notify_function =timer_thread;
	sigevt.sigev_notify_attributes = NULL;

	// populate timerspec
	timerspec.it_value.tv_sec = 5;
	timerspec.it_value.tv_nsec = 0;
	timerspec.it_interval.tv_sec = 0;
	timerspec.it_interval.tv_nsec =0;

	//creat timer
	timer_create(CLOCK_MONOTONIC,&sigevt,&timer);	

	// loop until we send all the packets
	while(i<=total_packets_to_send){

		//read chunks of PCKLEN(1024) from the file into readData where size of each object to be read (byte) is 1
		totalbytes = fread(readData, 1, PCKLEN, file);
		//cout<<"totalbytes: "<<totalbytes<<endl;
		if (totalbytes <= 0 )
		{
			cout<<"Read the whole file"<<endl;
			break;
		}
		


		//to continually resend the packet until we receive the ack we are expecting.
		//will take care of out of order acks sent, or duplicate acks.
		///when we recieve ACK, cancel the timer. 
		do{
			//send the packet
			send_data_packet(sock, DATA, seq_num, readData, totalbytes, client_addr, clisize);
			//start the timer
	                timer_settime(timer, 0, &timerspec,0); 	
			cout<<"we have set the timer"<<endl;
			
			//wait for recieving ACK
			void * receiveDataAck = malloc(PTR_SIZE);
			packet received;
			ret = recvfrom(sock, receiveDataAck, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
			if(ret < 0)
				error_msg("ack to begin file transfer failed");
			else
				cout<<"Recieved"<<endl;

                        received.deserialize(receiveDataAck);

			sequence_we_got= received.sequence_num;
			free(receiveDataAck);
			
			//we need to make sure the sequence numbers match, otherwise we resend
			if (received.type == DATA_ACK && sequence_we_got == seq_num)
			{
				cout<<"DATA_ACK received\n";
				++seq_num;
				break;//THIS IS NECESSARY OR WE SEND FOREVER
			}
		}while(sequence_we_got != seq_num);
		cout<<"i  "<<i<<endl;
		++i;//we can increment i here because we got past the dowhile.
	
	}*/ //THIS WAS ALL PROJECT 1 AND CAN BE DELETED ONCE THINGS WORK

	// begin file transfer
	clock_t begin = clock();
        //CREATE THE THREADS
        pthread_create(&sendthread, NULL, sender, NULL);
        pthread_create(&recvthread, NULL, receiver, NULL);


	// File transfer algo
	//
	//
	//
	//
	//

	//File transfer complete
	clock_t end = clock();


	//Time taken for file transfer in seconds
	total_time = (double)(end-begin)/CLOCKS_PER_SEC;
	
	// close connection
	cout<<"sending close request"<<endl;
	
	send_data_packet(sock, CLOSE, 0, buffer, PCKLEN, client_addr, clisize);
	void * close= malloc(PTR_SIZE);
	// receive close connection ack from the cluent
	ret = recvfrom(sock, close, PTR_SIZE, 0, (struct sockaddr*)&client_addr, &clisize);
	if(ret < 0)
		error_msg("Failed to receive close ack");

	cout<<"received close packet in server"<<endl;
	packet closepack;	
	closepack.deserialize(close);
	
	if(closepack.type == CLOSE){
		cout<<"close request accepted from client"<<endl;
		shutdown(sock,0);
	}
	else{
		cout<<"NOT A CLOSE PACKET"<<endl;	
	}

        /****************/
        //need to delete all the packets in the array!!
        




	//free the memory
//	free(readData);
	free(buffer);
	free(close);
//	// comment this free when files is closed

	free(file);
	return 0;
}

// function called when the timer expires. send the packet again to the client and wait for ack
void timer_thread(union sigval arg)
{
	cout<<"IN TIMER_THREAD"<<endl;
	//sigev_notify_function. Called once timer expires
	
	// acquire the lock
	pthread_mutex_lock(&mutex);
        for(int i =0; i<N; ++i)
        { 
          void * send = array[i].serialize();
	  if(sendto(globalsock, send, PTR_SIZE, 0, (struct sockaddr*) &globalclient, sizeof(globalclient))<0)
		cout<<"We could not send packet from timer:"<<endl;
	  else 
		cout<<"we sent from the timer"<<endl;
          ++retransmitted;
          ++totalpacketssent;
          
        }
        shiftedby = 0;

	// reset the timer	
	timer_settime(timer,0,&timerspec,0);

	// release the lock
	pthread_mutex_unlock(&mutex);

}

// function for error message
void error_msg(const char * message)
{
	perror(message);
	exit(1);
}

//function to send the packet to the client
void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size,
			sockaddr_in client_socket, socklen_t clilen)
{
	//create the packet
	packet mypacket(type, sequence_num, size, buffer);//pass info into constructor.
	//cout<<"SIZE OF DATA = "<<mypacket.size<<endl;
	void * to_send = mypacket.serialize();
	pthread_mutex_lock(&mutex);
	globalptr = to_send;
	if(sendto(sockID, to_send, PTR_SIZE, 0, (struct sockaddr*) &client_socket, clilen) < 0)
		cout<<"Send "<<type<<" failed"<<endl;
	pthread_mutex_unlock(&mutex);
	free(to_send);
}

//This function will be the function that executes on a single thread. Its purpose
//is to only send packets
void *sender(void * args)
{
 
        int totalbytes =0;
	float total_packets_to_send = ceil(gfilesize /1024.0);//the number of packets we will be sending rounded up
	cout<<"total packets to be sent" <<total_packets_to_send<<endl;
			
	//to loop while we transfer the files. 
	void * readData = malloc(PCKLEN);
	int seq_num = 4; // sequence number of the packet after initial syn, syn_ack, ack
	int i =0;
	int sequence_we_got = 0;

	int thread_param = 31415; //Parameter to pass
 

        sigevt.sigev_notify = SIGEV_THREAD;
	sigevt.sigev_value.sival_ptr = &thread_param;
	sigevt.sigev_notify_function =timer_thread;
	sigevt.sigev_notify_attributes = NULL;

	// populate timerspec
	timerspec.it_value.tv_sec = 5;
	timerspec.it_value.tv_nsec = 0;
	timerspec.it_interval.tv_sec = 0;
	timerspec.it_interval.tv_nsec =0;

	//creat timer
	timer_create(CLOCK_MONOTONIC,&sigevt,&timer); //this timer will need to retransmit each packet

	// loop until we send all the packets
	while(i<=total_packets_to_send){
                
		//read chunks of PCKLEN(1024) from the file into readData where size of each object to be read (byte) is 1
		totalbytes = fread(readData, 1, PCKLEN, gfile);
		//cout<<"totalbytes: "<<totalbytes<<endl;
		if (totalbytes <= 0 )
		{
			cout<<"Read the whole file"<<endl;
			break;
		}

        }

        free(readData);
}

//This function will be the function that executes on a single thread to implement
//the go-back-N protocol. This thread will continually read acks from the client
//and update the necessary variables within a lock.
void *receiver(void *args)
{

	while(1)
	{

		void * receiveAck= malloc(PTR_SIZE);	
		int ret = recvfrom(globalsock, receiveAck, PTR_SIZE, 0, (struct sockaddr*)&globalclient, &globalclisize);
		if(ret < 0)
			error_msg("Failed to receive data ack");

		packet rcv;	
		rcv.deserialize(receiveAck);
		cout<<"received ack in server:"<<rcv.sequence_num<<endl;
		free(receiveAck);
		
		pthread_mutex_lock(&mutex);
		if(rcv.type == DATA_ACK && rcv.sequence_num == acknum)
		{
			currentbase = acknum + 1;
			++acknum;		
			++shiftedby;
		}

		pthread_mutex_unlock(&mutex);
	}

}
