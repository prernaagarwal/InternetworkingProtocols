#include <iostream>
const int PCKLEN = 1024;//1kb MAX

//3way handshake structs
/*struct SYN{
	char type;
	int packlength;
	char *filename;
};

struct SYN_ACK{
	char type;
	double file_size;
};

struct ACK{
	char type;
};


//DATA STRUCTS
struct data{
	char type;
	long seqnum;
	int packlength;
	char * pckdata;
};

struct ackpack{
	char type;
	long seqnum;
};

struct close{
	char type;
	long seqnum;
};*/

//going to use a single packet and emun to simplify packets. 

typedef enum{
		SYN, //Connection request
		SYN_ACK, //Connection Acknowledge
		ACK,// Last bit of 3-way handshake. Acknowledge before transmitting data 
		DATA,//Data Packet
		DATA_ACK,//Data packet ack
		CLOSE,//Close connection request/ack
}packettype;

const size_t PTR_SIZE = sizeof(packettype)+sizeof(int)+sizeof(int)+PCKLEN;

//single packet for handshake requests, acks, and data req's and acks. 
struct packet{
	packet();	
	packet(packettype tp, int sq_num, int size_data, void * buff);
	packet(void * buff);
	void * serialize();
	void deserialize(void * buff);//deserialize a packet
        void update(packettype newtype,int seq_num, int newsize, void * buffer);
	~packet();

	packettype type;
	int sequence_num;
	int size;
	void * data;

};

