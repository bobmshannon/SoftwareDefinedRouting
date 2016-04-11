/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-11 19:31:32
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include "../include/response.h"

struct Response::control_response Response::build(uint32_t ip, uint8_t control_code,
                              uint8_t response_code,
                              std::vector<char> payload) {
  struct control_response resp = {};
  resp.ip = ip;
  resp.control_code = control_code;
  resp.response_code = response_code;
  resp.payload_length = payload.size();
  resp.payload = payload;
  return resp;
}
