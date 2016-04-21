#ifndef _ROUTING_TABLE_H_
#define _ROUTING_TABLE_H_

#include <stdint.h>

struct routing_table_entry {
	uint16_t destination_id;
	uint16_t padding;
	uint16_t next_hop_id;
	uint16_t cost;
};


#endif