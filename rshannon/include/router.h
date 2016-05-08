#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <stdint.h>
#include <vector>
#include <string>

#define INF 65535

#include "../include/routing_table.h"

using std::vector;
using std::string;
using std::pair;

class Router {
  
private:
	struct router {
		uint16_t id;
		uint16_t router_port;
		uint16_t data_port;
		uint16_t cost;
		uint32_t ip;
	};

	struct routing_update {
		uint32_t ip;
		uint16_t port;
		uint16_t padding;
		uint16_t id;
		uint16_t cost;
	};

	struct routing_update_pkt {
		uint16_t num_updates;
		uint16_t source_port;
		uint32_t source_ip;
		vector<struct routing_update> updates;
	};

	vector<router> routers;
	vector<struct routing_table_entry> routing_table;
	uint16_t update_interval;
	struct router this_router;

	/**
	 * Initialize routing table based on router
	 * information received from INIT command 
	 * sent by controller. This should be called
	 * only once.
	 */
	void init_routing_table();

	/**
	 * Convert a router ID to its corresponding IP
	 * 
	 * @return an IP address
	 */
	uint32_t id_to_ip(uint16_t id);

	/**
	 * Convert a router ID to its corresponding router port
	 * 
	 * @return a port
	 */
	uint16_t id_to_router_port(uint16_t id);


	/**
	 * Convert a routing update packet to vector<char> format
	 *
	 * @return a routing update packet
	 */
	vector<char> to_vector(struct routing_update_pkt pkt);

public:
	/**
	 * Initialize router core.
	 * 
	 * @param data The payload returned from the INIT command sent
	 * by the controller. This includes a description of each router
	 * in the network (see defn for struct router).
	 */
	void init(vector<char> data);

	/**
	 * Retrieve routing table.
	 */
	vector<char> get_routing_table_raw();
	vector<routing_table_entry> get_routing_table();

	/**
	 * Update routing table.
	 */
	void update_routing_table(vector<char> control_payload);

	/**
	 * Retrieve this router's ID
	 */
	uint16_t get_id();

	/**
	 * Retrieve this router's updates port.
	 */
	string get_router_port();

	/**
	 * Generate a routing update packet
	 */
	vector<char> get_routing_update();
};

#endif