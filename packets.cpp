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
	int off=0;
	memcpy(&(this->type), ((packettype*)buff+off), sizeof(type));
	off = sizeof(this->type);
	memcpy(&(this->sequence_num), ((int*)buff+off), sizeof(int));
	off += sizeof(int);
	memcpy(&(this->size), (int*)buff+off, sizeof(int));
	off += sizeof(int);
	memcpy(&(this->data), buff+off, PCKLEN);
/*
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
*/
	
}

void * packet::serialize()
{
	void * buff = malloc(PTR_SIZE);
	int off=0;
	memcpy(buff, &(this->type), sizeof(this->type));
	off = sizeof(this->type);
	memcpy(buff + off, &(this->sequence_num), sizeof(int));
	off += sizeof(int);
	memcpy(buff + off, &(this->size), sizeof(int));
	off += sizeof(int);
	memcpy(buff + off, &(this->data), PCKLEN);
	//std::cout<< *b;
	return buff;

}

packet::~packet()
{
	free(data);
}
