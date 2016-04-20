#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <stdint.h>
#include <vector>

using std::vector;

class Router {
  
private:
	struct router {
		uint16_t id;
		uint16_t router_port;
		uint16_t data_port;
		uint16_t cost;
		uint32_t ip;
	};

	std::vector<router> routers;
	uint16_t update_interval;
	struct router this_router;

public:
	/**
	 * Initialize router core.
	 * 
	 * @param data The payload returned from the INIT command sent
	 * by the controller. This includes a description of each router
	 * in the network (see defn for struct router).
	 */
	void init(vector<char> data);
};

#endif