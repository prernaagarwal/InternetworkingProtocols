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
	char * file = argv[3];

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
/*
	if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("Connection failed! \n"); 
		exit(0); 
	} 
	else
		printf("Connected!\n")
*/
	// ssize_t write(int fs, const void *buf, ssize_t N);
	// N bytes from buf to the file or socket associated with fs. N should not be greater than INT_MAX (defined in the limits.h header file). 
	// If N is zero, write() simply returns 0 without attempting any other action.

	/* ORIGINAL CODE TO SEND A FILE
	if (sendto(clientSocket, file, sizeof(file), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 printf( "sendto failed" );
	}

	cout <<"message sent: "<< file ;
	*/

	//sending SYN packet

	packet connect(SYN, 0, 0, (void*)calloc(1,PCKLEN));	
	void * ptr = connect.serialize();
	if (sendto(clientSocket, ptr, sizeof(ptr), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 cout<<"sendto failed\n";
	}

	cout <<"SYN packet sent";
	delete(ptr)	
		
	close(clientSocket);

	return 0;
}


