/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-16 14:48:21
*/

#include "../include/response.h"
#include <cstring>

using std::vector;

struct Response::control_response Response::build(uint8_t control_code,
                            bool err,
                            std::vector<char> payload) {
	struct control_response resp = {};
	resp.ip = htonl(controller_ip);
	resp.control_code = control_code;
	if(err) {
		resp.response_code = 1;
	} else {
		resp.response_code = 0;
	}
	resp.payload_length = htons(payload.size());
	resp.payload = payload;
	return resp;
}

vector<char> Response::to_vector(struct control_response resp) {
	struct control_response_header header = { resp.ip, resp.control_code, resp.response_code, resp.payload_length };
	char buf[sizeof(header)];
	vector<char> vector_resp;
	memcpy(buf, &header, sizeof(header));
	for(int i = 0; i < sizeof(header); i++) {
		vector_resp.push_back(buf[i]);
	}
	for(int i = 0; i < resp.payload.size(); i++) {
		vector_resp.push_back(resp.payload[i]);
	}
	return vector_resp;
}


Response::Response(std::string ip) {
    // http://stackoverflow.com/questions/15531402/how-to-convert-ip-address-both-ipv4-and-ipv6-to-binary-in-c
	struct sockaddr_in antelope;
	inet_pton(AF_INET, ip.c_str(), &(antelope.sin_addr));
	uint32_t address = antelope.sin_addr.s_addr;
	controller_ip = address;
}

Response::~Response() { }

vector<char> Response::author(bool err) {
	std::vector<char> statement;
	std::string statement_str = "I, rshannon, have read and understood the course academic integrity policy.";

	for(int i = 0; i < statement_str.size(); i++) {
		statement.push_back(statement_str[i]);
	}

	struct control_response resp = build(AUTHOR, err, statement);

	return to_vector(resp);
}