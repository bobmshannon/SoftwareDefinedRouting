#ifndef _NEIGHBOR_H_
#define _NEIGHBOR_H_

#include <stdint.h>

struct neighbor {
	uint32_t ip;
	uint16_t router_port;
	uint16_t data_port;
};

#endif