#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "../include/control_packet.h"
#include "../include/routing_table.h"

struct control_response;

using std::vector;

class Response {
private:
  uint32_t controller_ip;

  ////////////////////////////////////////////////////////////////////////////////////////////
  // CONTROLLER MESSAGE RESPONSES
  ////////////////////////////////////////////////////////////////////////////////////////////
  /**
   * The response format to a controller message.
   */
  struct control_response {
    uint32_t ip;           // IP address of the controller
    uint8_t control_code;  // The type of control message being responded to
    uint8_t response_code; // 0x00 indicates success, any other value indicates
                           // error
    uint16_t
        payload_length; // Size of the payload in bytes, excluding header fields
    vector<char> payload;
  };

  struct control_response_header {
    uint32_t ip;           // IP address of the controller
    uint8_t control_code;  // The type of control message being responded to
    uint8_t response_code; // 0x00 indicates success, any other value indicates
                           // error
    uint16_t
        payload_length; // Size of the payload in bytes, excluding header fields
  };

  /**
   * The payload for a ROUTING-TABLE (control code 0x02)
   * message response
   */
  struct routing_payload {
    vector<struct routing_entry> routing_entries;
  };

  struct routing_entry {
    uint16_t router_id;
    uint16_t padding;
    uint16_t next_hop_id;
    uint16_t cost;
  };

  /**
   * The payload for a SENDFILE-STATS (control code 0x06)
   * message response
   */
  struct statistics_payload {
    uint8_t transfer_id;
    uint8_t ttl;
    uint8_t padding;
    vector<uint16_t> sequence_numbers;
  };

  /**
   * The payload for a AUTHOR (control code 0x00)
   * message response
   */
  struct author_payload {
    vector<char> statement;
  };

  /**
   * Build a control message response.
   *
   * @param  ip            the IP address of the controller
   * @param  control_code  the type of message being responded to
   * @param  response_code the response code, 0x00 for success, failure
   * otherwise
   * @param  payload       the payload of the response
   * @return               a control message response
   */
  struct control_response build(uint8_t control_code,
                            bool err,
                            vector<char> payload);

  vector<char> to_vector(struct control_response);

public:
  Response(uint32_t ip = 0);
  ~Response();

  /**
   * Build a response to a ROUTING-TABLE controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  std::vector<char> routing_table(vector<routing_table_entry> routing_table, bool err = false);

  /**
   * Build a response to a SENDFILE-STATS controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  struct control_response sendfile_stats(bool err = false);

  /**
   * Build a response to a AUTHOR controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  std::vector<char> author(bool err = false);

  /**
   * Build a response to a CRASH controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  std::vector<char> crash(bool err = false);

  /**
   * Build a response to a SENDFILE controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  struct control_response sendfile(bool err = false);

  /**
   * Build a response to a UPDATE controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  std::vector<char> update(bool err = false);

  /**
   * Build a response to a PENULTIMATE-DATA-PACKET controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  struct control_response penultimate_data_packet(bool err = false);

  /**
   * Build a response to a LAST-DATA-PACKET controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  struct control_response last_data_packet(bool err = false);

  /**
   * Build a response to a INIT controller message
   *
   * @param  err whether the response should indicated an error. optional.
   * @return     a control message response
   */
  std::vector<char> init(bool err = false);

  /**
   * Set the controller IP address to be used when building
   * message responses
   *
   * @param  ip the controller IP address
   * @return    true if updated, false otherwise
   */
  bool set_controller(std::string ip);
};

#endif