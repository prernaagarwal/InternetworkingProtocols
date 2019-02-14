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
using namespace std;

//prototypes
void error_msg(const char * message);
void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size, sockaddr_in client_socket, socklen_t clilen);

//NO LONGER BEING USED
//bool myconnect(int sock, struct sockaddr_in client_socket, socklen_t clilen, packettype type);
//void send_ack_packet(int sockID, packettype type, int sequence_num, sockaddr_in client_socket, socklen_t clilen);

int main(int argc, char * argv[])
{
	int sock, newsock, portnum, ret; //clisize, ret;//file descriptors, port number, client address size,
	//and variable to caputer return values
	void* buffer;
	struct sockaddr_in serv_addr;//server address
	struct sockaddr_in client_addr;//client address
	socklen_t clisize;
	int n=0;
	int seq =0;
	char *filename;
	int filesize = 0;
	
	cout<<"In server"<<endl;

	if(argc < 2)
	{
		printf("No Port Number Provided");
		exit(1);
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	//bzero((char *) &serv_addr, sizeof(serv_addr));
	//bzero((char *) &client_addr, sizeof(client_addr));
	
	//set the port num to what was passed in. format server port
	portnum = atoi(argv[1]);	

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_msg("Cannot Open Socket");
	else
		printf("Socket now open \n");


	//set the servers information
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portnum);

	//bind socket with server
	if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error_msg("Error, could not bind.");
		exit(1);
	}
	else
		printf("We are now bound!\n");


	clisize = sizeof(client_addr);
	buffer = malloc(PTR_SIZE);//creating buffer for maximum packet length

/*    DOES NOT WORK
	
	void * synbuff = malloc(PTR_SIZE);

	n = recvfrom(sock,synbuff, PTR_SIZE, 0, (struct sockaddr*)&client_addr, &clisize); 
	if(n < 0)
		error_msg("Syn Recieve Failed.");
	
	
	packet recieved;
	recieved.deserialize(synbuff);
	cout<<"size of packet" <<sizeof(recieved)<<endl;
	
	
	
	if(recieved.type != SYN)
		error_msg("Expecting SYN packet.");
	else 
		cout<<"SYN ACCEPTED" <<endl;
*/


//	void * synackbuff = malloc(PCKLEN);
//	memset(synackbuff, 0,PCKLEN);
//	send_ack_packet(sock, SYN_ACK, 1, client_addr, clisize);

	
//	if(myconnect(sock, client_addr, clisize, SYN_ACK) != true)
//		error_msg("cant connect");
	
//	packet mypacket(SYN_ACK, 1, 0, (void *)calloc(1,PCKLEN));
//	void * to_send = mypacket.serialize();
	//cout<<" size: "<<sizeof(to_send)<<endl;
	//cout<<"SOCKID"<<sock <<endl;

//	 THE SEND WE WERE USING	
//	if(sendto(sock, synbuff, PTR_SIZE, 0, (struct sockaddr *)&client_addr, clisize ) < 0 )
//		cout<<"could not send synack"<<endl;
/*	
	void * mydata = malloc(PCKLEN);
	memset(mydata, 0, PCKLEN);	
	send_data_packet(sock, SYN_ACK, 2, mydata, PCKLEN, client_addr, clisize);	*/
		//will be the file request
	n = recvfrom(sock, buffer, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
	if(n < 0)
		error_msg("Recieve Failed");
	else
		cout<<"Recieved"<<endl;
	//	cout<<"buffer: "<<(char *)buffer<<endl;//casts the buffer to void *, shows the "filename" we recieved from client
		
		//check if we have the file in question

	packet filerequest;
	filerequest.deserialize(buffer);
	char * filerequested = (char *)filerequest.data;
	cout<<"file requested to char: "<<filerequested<<endl;
	if(filerequest.type == SYN){
		cout<<"SYN PACKET RECIEVED"<<endl;	
	}
	else
		error_msg("NOT A SYN PACKET.");



	if(access(filerequested, F_OK) == -1){
		error_msg("We do not have that file.");
	}
	else{
		cout<<"File has been found!" <<endl;
	}
		
		//need to send packet with file size
	FILE * file = fopen(filerequested, "r");


		//Gets the size of the file we want
	struct stat file_stat;
	stat(filerequested, &file_stat);
	filesize = file_stat.st_size;
	cout<<"file size = " <<filesize<<endl;


	//create pointer to point to the file size so that we may pass this back to the client in our SYN_ACK
	void * data = &filesize; //malloc(PCKLEN);
	//memcpy(data, &filesize, sizeof(int));//copies the filesize into the pointer to be sent as data	
	cout<<"FILE SIZE :::: "<<*(int *)data<<endl;
	send_data_packet(sock, SYN_ACK, 2, data, PCKLEN, client_addr, clisize);//sequence number of 2 since we've already recv'd reguest
	//cout<<"File Ack sent"<<endl;//confirmation we sent the ACK.

			
	//recv the ack from client before transmitting file. COMMENTED OUT THE RECVFROM AND DESERIALIZE FOR TIME BEING
	void * ack_to_begin = malloc(PTR_SIZE);
//	recvfrom(sock, ack_to_begin, PCKLEN, 0, (struct sockaddr*) &client_addr, &clisize);//recieve ack packet.
	packet beginpacket;
//	beginpacket.deserialize(ack_to_begin);
	/////////////////////////////////////
	
	bool flagTransfer = false;	
	n = recvfrom(sock, ack_to_begin, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
	if(n < 0)
		error_msg("ack to begin file transfer failed");
	else
		cout<<"Recieved"<<endl;
	beginpacket.deserialize(ack_to_begin);
	if (beginpacket.type == ACK)
		cout<<"ACK to transfer file received\n";
	
	flagTransfer = true;	
	cout<<"flagTransfer: "<<flagTransfer<<endl;
	
	int totalbytes =0;//total bytes we have sent to client. will let us know when to close the connection.
	float total_packets_to_send = ceil(filesize /1024.0);//the number of packets we will be sending.
	//float total_packets_to_send = filesize /1024;//the number of packets we will be sending.
	cout<<"total packets to be sent" <<total_packets_to_send<<endl;//I just wanted to see how many.
			
	//to loop while we transfer the files. 
	void * readData = malloc(PCKLEN);
	int seq_num = 4; 
	int i =1;
	while(i<=total_packets_to_send){
		//read from the file. Read into data with size PCKLEN(1024) bytes, up to the total_packets_to_send number of elements.	
		
		//?????????????????? data here is an int!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//totalbytes = fread(data, PCKLEN,total_packets_to_send, file);

		//read chunks of PCKLEN(1024) from the file into readData where size of each object to be read (byte) is 1
		totalbytes = fread(readData, 1, PCKLEN, file);
		cout<<"totalbytes: "<<totalbytes<<endl;
		if (totalbytes <= 0 )
		{
			cout<<"Read the whole file"<<endl;
			break;
		}
		

		//after we have read, we can send the packet
		send_data_packet(sock, DATA, seq_num, readData, totalbytes, client_addr, clisize);
		
		cout<<"Packets read and sent: "<<i<<endl;
		++i;
		
		

	
		//start our timer

		//wait for recieving ACK
			
		void * receiveDataAck = malloc(PCKLEN);
		packet received;
		n = recvfrom(sock, receiveDataAck, PTR_SIZE, 0, (struct sockaddr*) &client_addr, &clisize);
		if(n < 0)
			error_msg("ack to begin file transfer failed");
		else
			cout<<"Recieved"<<endl;
		received.deserialize(receiveDataAck);
		if (received.type == DATA_ACK)
		{
			cout<<"DATA_ACK received\n";
			++seq_num;
		}
		//when we recieve ACK, cancel the timer. 

		//check that the sequence numbers match

		//if they do not, resend our packet, if they do, start over. 
	
	}
	//end of while(1) for sending and recv packets. 
	

//	close(sock);

	return 0;
}

void error_msg(const char * message)
{
	perror(message);
	exit(1);
}
void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size,
			sockaddr_in client_socket, socklen_t clilen)
{
	//create the packet
	packet mypacket(type, sequence_num, size, buffer);//pass info into constructor.
	//serialize the packet
	void * to_send = mypacket.serialize();
	//sendto
	if(sendto(sockID, to_send, PCKLEN, 0, (struct sockaddr*) &client_socket, clilen) < 0)
		cout<<"Send "<<type<<" failed"<<endl;
}



/*
//send packet WITHOUT data
void send_ack_packet(int sockID, packettype type, int sequence_num, sockaddr_in client_socket,
 			socklen_t clilen)
{
	//create the packet
	//cout<<"Sending packet of " <<*(int *)data<<" size"<<endl;
	void * data = malloc(PCKLEN);
	memset(data, 0, PCKLEN);
//	cout<<"Sending SYN_ACK"<<endl;
	packet mypacket(type, sequence_num, 0, (void*)calloc(1,PCKLEN));	
	//serialize the packet
	void * to_send = mypacket.serialize();
	//sendto(sockID, to_send, sizeof(to_send), 0,(struct sockaddr*) &client_socket, clilen);
//	if(sendto(sockID, to_send, sizeof(to_send), 0, (struct sockaddr *)&client_socket, sizeof(client_socket)) < 0 )
		cout<<"send failed"<<endl;
	

}*/

/*
bool myconnect(int sock, struct sockaddr_in client_socket, socklen_t clilen, packettype type){
	cout<<"In connect func."<<endl;

	//void * data = malloc(PCKLEN);
	//memset(data, 0, PCKLEN);

	packet mypacket(type, 1, 0, (void *)calloc(1,PCKLEN));
	void * to_send = mypacket.serialize();
	//cout<<"SOCKID"<<sock <<endl;
	if(sendto(sock, to_send, sizeof(to_send), 0, (struct sockaddr *)&client_socket, clilen) < 0 ){
		cout<<"send failed"<<endl;
		return false;
	}
	cout<<"Packet successfully send"<<endl;
	return true;
	

}*/

