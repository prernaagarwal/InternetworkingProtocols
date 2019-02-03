#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for atoi() function: converts string to int
#include <sys/socket.h>

int main(int argc, char * argv[])
{
	/*
	 * Testing argc and argv
	printf( "argc = %d\n", argc );
	int i = 0;
	for( i = 0; i < argc; ++i)
	{
		printf( "argv[ %d ] = %s\n", i, argv[ i ] );
	}
	*/

	char * address = argv[1];
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
	
	return 0;
}
