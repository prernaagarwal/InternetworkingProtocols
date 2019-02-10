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
using namespace std;

//prototypes
void error_msg(const char * message);
void send_ack_packet(int sockID, packettype type, int sequence_num, void * data, sockaddr_in client_socket, socklen_t clilen);
void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size, sockaddr_in client_socket, socklen_t clilen);


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
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
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
	buffer = malloc(PCKLEN);//creating buffer for maximum packet length
//recieve connection
	
		//will be the file request
	n = recvfrom(sock, buffer, PCKLEN, 0, (struct sockaddr*) &client_addr, &clisize);
	if(n < 0)
		error_msg("Recieve Failed");
	else
		cout<<"Recieved"<<endl;
	//	cout<<"buffer: "<<(char *)buffer<<endl;//casts the buffer to void *, shows the "filename" we recieved from client
		
		//check if we have the file in question
	if(access((char *)buffer, F_OK) == -1){
		error_msg("We do not have that file.");
	}
	else{
		cout<<"File has been found!" <<endl;
	}
		
		
		//need to send packet with file size
	FILE * file = fopen((char *)buffer, "r");


		//Gets the size of the file we want
	struct stat file_stat;
	stat((char*)buffer, &file_stat);
	filesize = file_stat.st_size;
	cout<<"file size = " <<filesize<<endl;


	//create pointer to point to the file size so that we may pass this back to the client in our SYN_ACK
	void * data = malloc(PCKLEN);
	memcpy(data, &filesize, sizeof(int));//copies the filesize into the pointer to be sent as data	
	send_ack_packet(sock, SYN_ACK, 2, data, client_addr, clisize);//sequence number of 2 since we've already recv'd reguest
	cout<<"Ack sent"<<endl;//confirmation we sent the ACK.

			
	//recv the ack from client before transmitting file. COMMENTED OUT THE RECVFROM AND DESERIALIZE FOR TIME BEING
	void * ack_to_begin = malloc(PCKLEN);
//	recvfrom(sock, ack_to_begin, PCKLEN, 0, (struct sockaddr*) &client_addr, &clisize);//recieve ack packet.
	packet beginpacket;
//	beginpacket.deserialize(ack_to_begin);

	
	
	int totalbytes =0;//total bytes we have sent to client. will let us know when to close the connection.
	int total_packets_to_send = filesize /1024;//the number of packets we will be sending.
	cout<<"total packets to be sent" <<total_packets_to_send<<endl;//I just wanted to see how many.
			
	//to loop while we transfer the files. 
	while(1){
		//read from the file. Read into data with size PCKLEN(1024) bytes, up to the total_packets_to_send number of elements.	
		totalbytes = fread(data, PCKLEN,total_packets_to_send, file);
		
		//after we have read, we can send the packet
		
		//start our timer

		//wait for recieving ACK
		
		//when we recieve ACK, cancel the timer. 

		//check that the sequence numbers match

		//if they do not, resend our packet, if they do, start over. 
	
	}//end of while(1) for sending and recv packets. 
	

	close(sock);

	return 0;
}

void error_msg(const char * message)
{
	perror(message);
	exit(1);
}

void send_ack_packet(int sockID, packettype type, int sequence_num, void * data, sockaddr_in client_socket,
 			socklen_t clilen)
{
	//create the packet
	//cout<<"Sending packet of " <<*(int *)data<<" size"<<endl;
	packet mypacket(type, sequence_num, 0, data);	
	//serialize the packet
	void * to_send = mypacket.serialize();
	sendto(sockID, to_send, PCKLEN, 0,(struct sockaddr*) &client_socket, clilen);
	//sendto
	

}

void send_data_packet(int sockID, packettype type, int sequence_num, void* buffer, int size,
			sockaddr_in client_socket, socklen_t clilen)
{
	//create the packet
	packet mypacket(type, sequence_num, size, buffer);//pass info into constructor.
	//serialize the packet
	void * to_send = mypacket.serialize();
	//sendto
	sendto(sockID, to_send, PCKLEN, 0, (struct sockaddr*) &client_socket, clilen);
}