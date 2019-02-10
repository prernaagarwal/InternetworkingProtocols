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
	return b;

}

packet::~packet()
{
	free(data);
}
