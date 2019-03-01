#include "packets.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//constructor
packet::packet(){
  sequence_num =0;
  size=0;
  data = NULL;
//does nothing
}
packet::packet(packettype tp, int sq_num, int size_data, void * buff)
{
	this->type = tp;
	this->sequence_num = sq_num;
	this->size = size_data;
	this->data = malloc(PCKLEN);
	memcpy(this->data, buff, PCKLEN);
}
/*
packet:: packet(void * buff)
{
	int off=0;
	memcpy(&(this->type), ((packettype*)buff+off), sizeof(type));
	off = sizeof(this->type);
	memcpy(&(this->sequence_num), ((int*)buff+off), sizeof(int));
	off += sizeof(int);
	memcpy(&(this->size), (int*)buff+off, sizeof(int));
	off += sizeof(int);
	this->data = malloc(PCKLEN);
	memcpy(&(this->data), buff+off, PCKLEN);


}
*/

void packet::deserialize(void * buff)
{
	int off=0;
	memcpy(&(this->type), (buff), sizeof(packettype));
	off = sizeof(packettype);
	memcpy(&(this->sequence_num), (buff+off), sizeof(int));
	off += sizeof(int);
	memcpy(&(this->size), (buff+off), sizeof(int));
	off += sizeof(int);
	this->data = malloc(PCKLEN);
	memcpy((this->data), buff+off, PCKLEN);
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
	memcpy(buff + off, (this->data), PCKLEN);
	//std::cout<< *b;
	return buff;

}

//updates the current information in a packet
void packet ::update(packettype newtype, int seq_num, int newsize, void * buffer)
{
  this->type = newtype;
  this->sequence_num =seq_num;
  this->size = newsize;
  if(this->data)
    free(this->data);
  this->data =malloc(PCKLEN);
  memcpy(this->data, buffer, PCKLEN);
}

packet::~packet()
{
	free(data);
}
