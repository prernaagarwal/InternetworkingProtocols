#include "packets.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//constructor
packet::packet(packettype tp, int sq_num, int size_data, void * buff)
{
	type = tp;
	sequence_num = sq_num;
	size = size_data;
	data = malloc(sizeof(&buff));
	memcpy(data, buff, sizeof(&buff));
}

packet::~packet()
{
	free(data);
}
