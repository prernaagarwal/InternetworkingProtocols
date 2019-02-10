#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "packets.h"
using namespace std;

//prototypes
void error_msg(const char * message);


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
	
	while(1){
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
/*		filename = (char *)buffer;	
		printf("%s",filename);*/
		
			//command line. 
	//need to send packet with file size
	//recv the ack from client before transmitting.

	//	packet mypack;
	//	mypack.deserialize(buffer);
	
	}//end of while(1) for sending and recv packets. 
	

	close(sock);
//reply once we get connection request 
//recieve connection ack

//then start sending files.


	//sleep(10);
	//LISTEN
	/*int status = listen(sock, 5);//file descriptor and size of backlog =7.
	printf("%d AND %d", sock, status);
	if(status == -1)
	{
		error_msg("Error while listening");
	}*/
	
	//sleep(10);
	//ACCEPT

/*	clisize = sizeof(client_addr);
	newsock = myaccept(sock, (struct sockaddr *) &client_addr, &clisize);
	if(newsock < 0)
		error_msg("Error, Could not accept");
	else
		printf("CONNECTED");

	bzero(buffer, 256);
	ret = read(newsock, buffer, 255);
	if(ret < 0)
		error_msg("Error reading from socket");
	else
		printf("The message: %s", buffer);
	ret = write(newsock, "Message Recieved",16);
	if(ret < 0)
		error_msg("Error writing to socket");
*/
	

	return 0;
}

void error_msg(const char * message)
{
	perror(message);
	exit(1);
}
/*	
int myaccept(int sockid, struct sockaddr * clientaddr, socklen_t clientlen)
{
	return 0;
}*/

void send_ack_packet(int sockID, packettype type, int *sequence_num)
{
	void * data = malloc(PCKLEN);
	memset(data, 0, PCKLEN);
	//create the packet
	//serialize the packet
	//sendto
	//deallocate memory
}

void send_data_packet(int sockID, packettype type, int *sequence_num, void* buffer, int size)
{
	//create the packet
	//serialize the packet
	//sendto
	//deallocate memory
}
