#include "packets.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//constructor
packet::packet(){
//does nothing
}
packet::packet(packettype tp, int sq_num, int size_data, void * buff)
{
	type = tp;
	sequence_num = sq_num;
	size = size_data;
	data = malloc(PCKLEN);
	memcpy(data, buff, PCKLEN);
}

void packet::deserialize(void * buff)
{
	//int off=0;
	//memcpy(this->type, &(buff), sizeof(this->type));
	//off = sizeof(this->type);
	//buff += off;
	//memcpy(sequence_num, &(buff), sizeof(int));
	//off += sizeof(int);
	//buff+=off;
	//memcpy(size, &(buff), sizeof(int));
	//off += sizeof(int);
	//buff+=off;

	packettype *q = (packettype*)buff;    
	this->type = *q;       
	q++;    
	int * p = (int *)q;
	this->sequence_num = *p;
	p++;
	this->size = *p;
	p++;
	void * pack = (void *)p;
	memcpy(this->data, &(pack), PCKLEN);

	
}

void * packet::serialize()
{
	void * b = malloc(sizeof(type)+sizeof(int)+sizeof(int)+sizeof(PCKLEN));
	int off=0;
	memcpy(b, &(this->type), sizeof(this->type));
	off = sizeof(this->type);
	memcpy(b + off, &(this->sequence_num), sizeof(int));
	off += sizeof(int);
	memcpy(b + off, &(this->size), sizeof(int));
	off += sizeof(int);
	memcpy(b + off, &(this->data), PCKLEN);
	//std::cout<< *b;
	return b;

}

packet::~packet()
{
	free(data);
}
