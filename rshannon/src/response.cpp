/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-24 15:59:22
*/

#include "../include/response.h"
#include "../include/error.h"
#include <cstring>

using std::vector;

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
Response::Response(uint32_t ip) {
    // http://stackoverflow.com/questions/15531402/how-to-convert-ip-address-both-ipv4-and-ipv6-to-binary-in-c
	/*struct sockaddr_in antelope;
	inet_pton(AF_INET, ip.c_str(), &(antelope.sin_addr));
	uint32_t address = antelope.sin_addr.s_addr;*/
	controller_ip = ip;
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

vector<char> Response::init(bool err) {
	std::vector<char> payload;
	struct control_response resp = build(INIT, err, payload);
	return to_vector(resp);
}

vector<char> Response::routing_table(vector<routing_table_entry> routing_table, bool err) {
	vector<char> payload;
	for(int i = 0; i < routing_table.size(); i++) {
		payload.push_back( (routing_table[i].destination_id >> 8) & 0x00FF);
		payload.push_back(routing_table[i].destination_id & 0x00FF);
		payload.push_back(0);
		payload.push_back(0);
		payload.push_back( (routing_table[i].next_hop_id >> 8) & 0x00FF);
		payload.push_back(routing_table[i].next_hop_id & 0x00FF);
		payload.push_back( (routing_table[i].cost >> 8) & 0x00FF);
		payload.push_back(routing_table[i].cost & 0x00FF);
		DEBUG("destination_id: " << routing_table[i].destination_id << " next_hop_id: " << routing_table[i].next_hop_id << " cost: " << routing_table[i].cost);
	}

	struct control_response resp = build(ROUTING_TABLE, err, payload);

	return to_vector(resp);
}