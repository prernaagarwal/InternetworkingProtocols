#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for atoi() function: converts string to int
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packets.h"
using namespace std;

bool myConnection(int clientSocket, struct sockaddr_in serv_addr, packettype type);
int main(int argc, char * argv[])
{
    	
	if (argc < 4)
	{
		cout<< "Missing arguments. Exiting...\n";
		exit(0);
	}

	struct sockaddr_in serv_addr;
    
	char buffer[256];
	int server_port = atoi(argv[2]);

	// Create a socket
	// socket(domain, type, protocol)
	int clientSocket = socket(AF_INET, SOCK_DGRAM,0);
	if (clientSocket < 0)
	{
		cout<< "Socket Connection failed!\n";
		return 0;
	}
    	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	

	//The inet_pton() function converts an Internet address in its standard text format into its numeric binary form. The argument af specifies the family of the address.
	if(inet_pton(AF_INET,argv[1],&(serv_addr.sin_addr)) < 0)
	{
		cout<<"Error: Invalid address \n"; 
		exit(0);
	}
	
/* 	 ORIGINAL CODE TO SEND A FILENAME
	if (sendto(clientSocket, file, sizeof(file), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 printf( "sendto failed" );
	}

	cout <<"message sent: "<< file ;
*/

	//sending SYN packet
 	
	if (myConnection(clientSocket, serv_addr, SYN))
	{
		cout<<"Connection with Server established Successfully!\n";
	}
	else
	{
		cout<<"Three way handshake connection failed!\n";
		
	}

	//File request
	char * file = argv[3];
	cout <<"Requested File: "<<file << endl;	

	packet filerequest(ACK,1,sizeof(file), file);
	void * fileptr = filerequest.serialize();

	if (sendto(clientSocket, fileptr, sizeof(fileptr), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 cout<<"sending filename failed\n";

	}

	cout <<"file requested from the server";
	//free(fileptr);	











	cout<<"File received\n";

	close(clientSocket);

	return 0;
}

bool myConnection(int clientSocket, struct sockaddr_in serv_addr, packettype type)
{

	//sending SYN packet
	packet connect(type, 0, 0, (void*)calloc(1,PCKLEN));	
	void * sendptr = connect.serialize();
	if (sendto(clientSocket, sendptr, sizeof(sendptr), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 cout<<"sendto failed\n";
		 return false;
	}

	cout <<"SYN packet sent\n";
	//delete(sendptr);	

	//receiving SYN+ACK packet
	void * rcvptr = malloc(PTR_SIZE);
	memset(rcvptr, 0, PTR_SIZE);
	int  serverlen = sizeof(serv_addr);
	if (recvfrom(clientSocket, rcvptr,  PTR_SIZE, 0, (struct sockaddr *)&serv_addr, (socklen_t*)&serverlen) < 0 )
	{
		cout<<"Receivefrom failed!\n";
		return false;
	}

	packet received;
	received.deserialize(rcvptr);

	if(received.type != SYN_ACK) {
		cout<<"SYN_ACK not received!\n";
		return false;
	}
	//free(rcvptr);

	return true;

}
