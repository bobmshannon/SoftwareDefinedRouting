#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>

#define INF 65535

#include "../include/routing_table.h"

using std::vector;
using std::string;

class Router {
  
private:
	struct router {
		uint16_t id;
		uint16_t router_port;
		uint16_t data_port;
		uint16_t cost;
		uint32_t ip;
	};

	vector<router> routers;
	vector<struct routing_table_entry> routing_table;
	uint16_t update_interval;
	struct router this_router;

	/**
	 * Update current routing table based on known router
	 * information.
	 */
	void build_routing_table();

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
	vector<char> get_routing_table();

	/**
	 * Retrieve this router's ID
	 */
	uint16_t get_id();

	/**
	 * Retrieve this router's updates port.
	 */
	string get_router_port();
};

#endif