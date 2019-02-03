#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for atoi() function: converts string to int
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


int main(int argc, char * argv[])
{
    	
	if (argc < 3)
	{
		printf("Missing arguments. Exiting...\n");
		exit(0);
	}

	struct sockaddr_in serv_addr;
    
	//char * address = argv[1];
	int server_port = atoi(argv[2]);
//	printf( "argv[ %d ] = %d\n", 2, server_port);

	// socket(domain, type, protocol)
	int clientSocket = socket(AF_INET, SOCK_DGRAM,0);
	if (clientSocket < 0)
	{
		printf("Socket Connection failed!");
		return 0;
	}
	printf("clientSocket: %d\n", clientSocket);
    	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	
	//The inet_pton() function converts an Internet address in its standard text format into its numeric binary form. The argument af specifies the family of the address.
	if(inet_pton(AF_INET,argv[1],&(serv_addr.sin_addr)) < 0)
	{
		printf("Error: Invalid address \n"); 
		exit(0);
	}

	if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("Connection failed! \n"); 
		exit(0); 
	} 


	return 0;
}
