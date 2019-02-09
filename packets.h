
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
		DATAACK,//Data packet ack
		CLOSE,//Close connection request/ack
}packettype;

//single packet for handshake requests, acks, and data req's and acks. 
typedef struct{
	//this isnt C++ so i can't actually do this.
	void * serialize(packettype tp, int sq_num, int size_data, void * buff);
	void * deserialize(void * buff);

	packettype type;
	int sequence_num;
	int size;
	void * data;
}packet;

