/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-21 15:12:38
*/
#include "../include/router.h"
#include <sstream>
#include <cstring>

using std::ostringstream;

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
void Router::init_routing_table() {
	for(int i = 0; i < routers.size(); i++) {
		if(routers[i].cost == INF) { continue; }
		struct routing_table_entry rte = {
			routers[i].id,	// Destination router ID
			0,				// Padding
			routers[i].id,	// Next hop router ID
			routers[i].cost // Cost
		};
		routing_table.push_back(rte);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
void Router::init(vector<char> data) {
	update_interval = (data[2] << 8) | (data[3]);

	int num_routers = (data[0] << 8) | (data[1]);

	for(int i = 4; i < num_routers; i += sizeof(router)) {
		struct router router = {
			(data[i] << 8) | (data[i+1]),	// ID
			(data[i+2] << 8) | (data[i+3]),	// ROUTER PORT
			(data[i+4] << 8) | (data[i+i+5]),	// DATA PORT
			(data[i+6] << 8) | (data[7]),	// COST
			(data[i+8] << 24)	| (data[i+9] << 16) | (data[i+10] << 8) | (data[i+11])	// IP
		};
		routers.push_back(router);
		if(router.cost == 0) { this_router = router; }
	}

	init_routing_table();
}

uint16_t Router::get_id() {
	return this_router.id;
}

string Router::get_router_port() {
	ostringstream port;
	port << this_router.router_port;
	return port.str();
}

vector<char> Router::get_routing_table() {
	vector<char> ret;

	for(int i = 0; i < routing_table.size(); i++) {
		struct routing_table_entry rte = routing_table[i];
		char buf[sizeof(rte)];
		memcpy(buf, &rte, sizeof(rte));
		for(int j = 0; j < sizeof(rte); j++) {
			ret.push_back(buf[j]);
		}		
	}

	return ret;
}