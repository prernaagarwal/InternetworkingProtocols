
//3way handshake structs
struct SYN{
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
};
