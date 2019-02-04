#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for atoi() function: converts string to int
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <packets.h>


int main(int argc, char * argv[])
{
    	
	if (argc < 4)
	{
		printf("Missing arguments. Exiting...\n");
		exit(0);
	}

	struct sockaddr_in serv_addr;
    
	char buffer[256];
	int server_port = atoi(argv[2]);
	char * file = argv[3];

	// socket(domain, type, protocol)
	int clientSocket = socket(AF_INET, SOCK_DGRAM,0);
	if (clientSocket < 0)
	{
		printf("Socket Connection failed!");
		return 0;
	}
//	printf("clientSocket: %d\n", clientSocket);
    	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
/*
	if (bind(clientSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("bind failed");
			return 0;
	}
	else
	{
		printf("bind successful\n");
	}
*/

	//The inet_pton() function converts an Internet address in its standard text format into its numeric binary form. The argument af specifies the family of the address.
	if(inet_pton(AF_INET,argv[1],&(serv_addr.sin_addr)) < 0)
	{
		printf("Error: Invalid address \n"); 
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
	if (sendto(clientSocket, file, sizeof(file), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		 printf( "sendto failed" );
	}
	printf( "message sent: %s\n", file );
/*
	if (write(clientSocket,file,strlen(file)))
	{
		printf("Can't write to socket!");
	}

   	
	bzero(buffer,256); // reset buffer to 0
       	
	if (read(clientSocket,buffer,255) < 0)
	{
		printf("ERROR reading from socket");
	}
	printf("%s\n",buffer);
*/
	close(clientSocket);

	return 0;
}
