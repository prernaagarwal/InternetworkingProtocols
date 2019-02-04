#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>


//prototypes
void error_msg(const char * message);


int main(int argc, char * argv[])
{
	int sock, newsock, portnum, ret; //clisize, ret;//file descriptors, port number, client address size,
	//and variable to caputer return values
	//char buffer[256];
	struct sockaddr_in serv_addr;//server address
	struct sockaddr_in client_addr;//client address
	socklen_t clisize;

	if(argc < 2)
	{
		printf("No Port Number Provided");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	bzero((char *) &client_addr, sizeof(client_addr));
	
	//set the port num to what was passed in. format server port
	portnum = atoi(argv[1]);	
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_msg("Cannot Open Socket");
	else
		printf("Socket now open");


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
		printf("We are now bound!");
	//sleep(10);
	//LISTEN
	int status = listen(sock, 5);//file descriptor and size of backlog =7.
	if(status == -1)
	{
		error_msg("Error while listening");
	}
	else
		printf("Listening")	
	sleep(10);
	//ACCEPT
	clisize = sizeof(client_addr);
	newsock = accept(sock, (struct sockaddr *) &client_addr, &clisize);
	if(newsock < 0)
		error_msg("Error, Could not accept");
	else
		printf("CONNECTED");

	return 0;
}

void error_msg(const char * message)
{
	perror(message);
	exit(1);
}

