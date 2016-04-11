#ifndef _Response_H_
#define _Response_H_

#include <stdint.h>
#include <vector>

class Response {
  private:
    ////////////////////////////////////////////////////////////////////////////////////////////
    // CONTROLLER MESSAGE RESPONSES
    ////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * The response format to a controller message.
     */
    struct control_response {
        uint32_t ip;    // IP address of the controller
        uint8_t control_code;   // The type of control message being responded to
        uint8_t response_code;  // 0x00 indicates success, any other value indicates error
        uint16_t payload_length;    // Size of the payload in bytes, excluding header fields
        std::vector<char> payload;
    };

    /**
     * The payload for a ROUTING-TABLE (control code 0x02) 
     * message response
     */
    struct routing_payload {
        std::vector<struct routing_entry>  routing_entries; 
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
        std::vector<uint16_t> sequence_numbers;
    };

  public:
    /**
     * Build a control message response.
     * 
     * @param  ip            the IP address of the controller
     * @param  control_code  the type of message being responded to
     * @param  response_code the response code, 0x00 for success, failure otherwise
     * @param  payload       the payload of the response
     * @return               a control message response
     */
    struct control_response build(uint32_t ip, uint8_t control_code, uint8_t response_code, std::vector<char> payload);
    
    struct control_response routing_table();

    struct control_response sendfile_stats();
};

#endif