#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

class Controller {
  
private:
	uint32_t controller_ip;
	uint8_t extract_control_code(vector<char> msg);
public:
	Controller();
	~Controller();
	vector<char> generate_response(vector<char> msg);
	void set_ip(uint32_t ip);
};

#endif