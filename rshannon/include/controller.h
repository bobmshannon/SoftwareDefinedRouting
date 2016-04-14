#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include <vector>

using std::vector;

class Controller {
  
private:
	uint8_t extract_control_code(vector<char> msg);
public:
	Controller();
	~Controller();
	vector<char> generate_response(vector<char> msg);
};

#endif