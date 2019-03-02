//Amanda Williams and Prerna Agarwal
// CS494, Project Part 2

#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for atoi() function: converts string to int
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packets.h"
#include <math.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char * argv[])
{

	//take in arguments from the command line.
	//client, ip address, port number, file to be requested from the server	
	if (argc < 4)
	{
		cout<< "Missing arguments. Exiting...\n";
		exit(0);
	}


	struct sockaddr_in serv_addr, client_addr;
	// convert string to int
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

	//Populater the serv_addr struct
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	

	//Check the ip address
	//The inet_pton() function converts an Internet address in its standard text format into its numeric binary form. 
	//The argument AF specifies the family of the address.
	if(inet_pton(AF_INET,argv[1],&(serv_addr.sin_addr)) == 0)
	{
		cout<<"Error: Invalid address \n"; 
		exit(0);
	}


	//File request

	char * file = argv[3];
	int length = strlen(file)+1;
	cout <<"Requested File: "<<file << endl;	

	//It works without void pointer. 
	//If errors, use this void pointer instead of char *	
	void * filename = malloc(PCKLEN);
	memset(filename, 0, PCKLEN);
	memcpy(filename, file, length);

	packet filerequest(SYN,1, length, filename);

	//cout<<"File:: "<<(char*)filerequest.data<<endl;
	void * fileptr = filerequest.serialize();

	// send the packet to the server to start three-way handshake
	if (sendto(clientSocket, fileptr, PTR_SIZE, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		cout<<"sending filename failed\n";

	}
	else
		cout <<"file requested from the server\n";
	free(fileptr);	

	//Receive file acknowledgement fromt the server. SYN_ACK
	void * rcvptr = malloc(PTR_SIZE);
	socklen_t serverlen = sizeof(serv_addr);
	if (recvfrom(clientSocket, rcvptr,  PTR_SIZE, 0, (struct sockaddr *)&serv_addr, &serverlen) < 0 )
	{
		cout<<"Receivefrom failed!\n";
	}

	packet received;
	received.deserialize(rcvptr);
	free(rcvptr);

	//cout<<received.type<<" , "<< " SIZE " <<

	//check for syn_ack
	if(received.type == SYN_ACK)
		cout<< "The Size of the file requested is: "<<*(int *)received.data<<endl;
	int totaltoread = *(int*)received.data;	

	// Complete the three way handshake
	// Sending file ack to the server //
	void * nodata = malloc(PCKLEN);
	memset(nodata, 0, PCKLEN);
	packet fileAck(ACK,2, 0, nodata);

	//cout<<"File:: "<<(char*)filerequest.data<<endl;
	void * fileSizeAck = fileAck.serialize();

	if (sendto(clientSocket, fileSizeAck, PTR_SIZE, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
	{
		cout<<"sending file size ack failed\n";
	}
	else
		cout <<"file size ack sent to the server\n";

	free(nodata);
	free(fileSizeAck);	

	cout<<"Ready to receive the file"<<endl;

	//FILE * fp;
	//fp = fopen("test.jpg.out" , "wb" );
	// Open the file to write
	int fp = open("test.jpg.out", O_CREAT | O_TRUNC | O_WRONLY | O_EXCL,S_IRWXU);	


	void * rcv = malloc(PTR_SIZE);
	int seq_num = -1; // used to store received packet's sequence
	int expected_seq_num = 4; // on server side, we are starting with seq num = 4
	int bytesWritten=0;
	int totalWritten =0;
	bool flag = false;
	while(1)
	{

		//receive data packet from the server
		if (recvfrom(clientSocket, rcv,  PTR_SIZE, 0, (struct sockaddr *)&serv_addr, &serverlen) < 0 )
		{
			cout<<"Receivefrom failed!\n";
		}
		packet receiveData;
		receiveData.deserialize(rcv);

		seq_num = receiveData.sequence_num;
		cout<<"Received: "<<seq_num<<" Expected: "<<expected_seq_num<<endl;	
		//check the sequence number of the packet received
		if(receiveData.sequence_num == expected_seq_num && receiveData.size <=1024)
		{
			expected_seq_num += 1;
			//write the bytes to the files
			flag = true;
			bytesWritten = write(fp, receiveData.data, receiveData.size);
			cout<< "Bytes Written = "<<receiveData.size<<endl;
			totalWritten+=receiveData.size;
		}//else if we got the same packet twice, dont rewrite but resend the packet.
		else
		{
			flag = false;
		}	
		//Send the packet received acknowledgement to the server
		void * nodata = malloc(PCKLEN);
		memset(nodata, 0, PCKLEN);

		//cout<<"File:: "<<(char*)filerequest.data<<endl;
		void * confirmed;


		//received sequence number DOESNT match the expected sequence number
		if(!flag)
		{

			packet confirmData(DATA_ACK,expected_seq_num - 1, 0, nodata);
			confirmed = confirmData.serialize();
		}
		//received sequence number matches the expected sequence number
		else
		{
			packet confirmData(DATA_ACK, seq_num, 0, nodata);
			confirmed = confirmData.serialize();
		}


		if (sendto(clientSocket, confirmed, PTR_SIZE, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) 
		{
			cout<<"sending data_ack failed\n";
		}
		free(nodata);
		free(confirmed);	
		//received all the bytes
		if(totalWritten == totaltoread)
			break;
	}

	//RECEIVE CLOSE REQUEST
	void * closeptr = malloc(PTR_SIZE);
	cout<<"waiting to receive close"<<endl;
	if (recvfrom(clientSocket, closeptr,  PTR_SIZE, 0, (struct sockaddr *)&serv_addr, &serverlen) < 0 )
	{
		cout<<"Close Receive failed."<<endl;
	}

	//send close request acknowledgement
	packet close;
	close.deserialize(closeptr);
	//correct close packet received
	if(close.type == CLOSE)
	{
		closeptr = close.serialize();
		cout<<"sending closing request ack"<<endl;
		//send failed
		if(sendto(clientSocket, closeptr, PTR_SIZE, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
		{
			cout<<"Sending Close Ack Failed"<<endl;
		}	

		shutdown(clientSocket,0);
	}
	else
		cout<<"NOT A CLOSE PACKET"<<endl;

	free(rcv);
	free(closeptr);

	return 0;
}
