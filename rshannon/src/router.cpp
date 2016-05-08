/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-05-07 21:28:14
*/
#include "../include/router.h"
#include "../include/error.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::ostringstream;

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
void Router::init_routing_table() {
	for(int i = 0; i < routers.size(); i++) {

		if(routers[i].cost == INF) {
			struct routing_table_entry rte = {
				routers[i].id,	// Destination router ID
				0,				// Padding
				INF,			// Next hop router ID
				INF 			// Cost
			};
			routing_table.push_back(rte);
		} else {
			struct routing_table_entry rte = {
				routers[i].id,	// Destination router ID
				0,				// Padding
				routers[i].id,	// Next hop router ID
				routers[i].cost // Cost
			};
			routing_table.push_back(rte);
		}
	}
}

uint32_t Router::id_to_ip(uint16_t id) {
	for(int i = 0; i < routers.size(); i++) {
		if(routers[i].id == id) { return routers[i].ip; }
	}
}

uint16_t Router::id_to_router_port(uint16_t id) {
	for(int i = 0; i < routers.size(); i++) {
		if(routers[i].id == id) { return routers[i].router_port; }
	}
}

vector<char> Router::to_vector(struct routing_update_pkt pkt) {
	char buf[sizeof(pkt)];
	vector<char> vector_pkt;
	memcpy(buf, &pkt, sizeof(pkt));
	for(int i = 0; i < sizeof(pkt); i++) {
		vector_pkt.push_back(buf[i]);
	}
	return vector_pkt;
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
void Router::init(vector<char> data) {
	update_interval = (data[2] << 8) | (data[3]);

	int num_routers = (data[0] << 8) | (data[1]);

	for(int i = 4; i < data.size(); i += sizeof(router)) {
		struct router router = {
			((data[i] << 8) & 0xff00) | (data[i+1] & 0xff),	// ID
			((data[i+2] << 8) & 0xff00) | (data[i+3] & 0xff),	// ROUTER PORT
			((data[i+4] << 8) & 0xff00) | (data[i+5] & 0xff),	// DATA PORT
			((data[i+6] << 8) & 0xff00) | (data[i+7] & 0xff),	// COST
			((data[i+8] << 24) & 0xff000000) | ((data[i+9] << 16) & 0xff0000) | ((data[i+10] << 8) & 0xff00) | ((data[i+11]) & 0xff)	// IP
		};

		routers.push_back(router);
		if(router.cost == 0) { this_router = router; }
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &router.ip, ip_str, INET_ADDRSTRLEN);
		DEBUG("Router | ID: " << router.id << " | Router Port: " << router.router_port << " | Data Port: " << router.data_port << " | Cost: " << router.cost << " | IP: " << router.ip << " " << ip_str);
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

vector<routing_table_entry> Router::get_routing_table() {
	return routing_table;
}

void Router::update_routing_table(vector<char> control_payload) {
    uint16_t router_id = ((control_payload[0] << 8) & 0xFF00) | ((control_payload[1]) & 0xFF);
    uint16_t new_cost = ((control_payload[2] << 8) & 0xFF00) | ((control_payload[3]) & 0xFF);

    for(int i = 0; i < routing_table.size(); i++) {
    	if(routing_table[i].destination_id == router_id) {
    		routing_table[i].cost = new_cost;
    	}
    }
    for(int i = 0; i < routers.size(); i++) {
    	if(routers[i].id == router_id) {
    		routers[i].cost = new_cost;
    	}
    }
    DEBUG("routing table updated. router_id: " << router_id << " new_cost: " << new_cost);
    for(int i = 0; i < routing_table.size(); i++) {
		DEBUG("destination_id: " << routing_table[i].destination_id << " next_hop_id: " << routing_table[i].next_hop_id << " cost: " << routing_table[i].cost);
    }   
}

vector<char> Router::get_routing_table_raw() {
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

vector<char> Router::get_routing_update() {
	struct routing_update_pkt update_pkt;
	update_pkt.num_updates = routing_table.size();
	update_pkt.source_port = this_router.router_port;
	update_pkt.source_ip = this_router.ip;

	for(int i = 0; i < routing_table.size(); i++) {
		struct routing_update routing_update;
		routing_update.ip = id_to_ip(routing_table[i].destination_id);
		routing_update.port = id_to_router_port(routing_table[i].destination_id);
		routing_update.padding = 0;
		routing_update.id = routing_table[i].destination_id;
		routing_update.cost = routing_table[i].cost;
		update_pkt.updates.push_back(routing_update);
	}

	return to_vector(update_pkt);
}