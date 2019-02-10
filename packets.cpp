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

void * packet::serialize(){

}

packet::~packet()
{
	free(data);
}
