#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include <vector>
#include <string>
#include "../include/router.h"
#include "../include/routing_table.h"
#include "../include/data.h"
#include "../include/controller.h"
#include "../include/control_packet.h"
#include "../include/response.h"
#include "../include/tcp_server.h"
#include "../include/udp_server.h"
#include "../include/udp_client.h"
#include "../include/error.h"

using std::vector;
using std::string;

class Controller {
  
private:
	uint32_t controller_ip;
	uint8_t extract_control_code(vector<char> msg);
	vector<char> extract_payload(vector<char> msg);
	bool running;
    Router router;
    Data data;
    UDPServer updates_server;
    TCPServer control_server;
    UDPClient udp_client;
    void process_routing_update();
    void process_control_msg();
    void broadcast_routing_updates();
public:
	Controller();
	~Controller();
	void start(string control_port);
	void stop();
	vector<char> generate_response(vector<char> msg);
	void set_ip(uint32_t ip);
};

#endif