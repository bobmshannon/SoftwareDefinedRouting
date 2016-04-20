#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include <vector>
#include <string>
#include "../include/router.h"
#include "../include/data.h"

using std::vector;
using std::string;

class Controller {
  
private:
	uint32_t controller_ip;
	uint8_t extract_control_code(vector<char> msg);
	vector<char> extract_payload(vector<char> msg);
	bool running;
public:
	Controller();
	~Controller();
	void start(string control_port);
	void stop();
	vector<char> generate_response(vector<char> msg);
	void set_ip(uint32_t ip);
};

#endif