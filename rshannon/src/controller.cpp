/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:41:26
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-20 00:22:18
*/

#include "../include/controller.h"
#include "../include/control_packet.h"
#include "../include/response.h"
#include "../include/tcp_server.h"
#include "../include/udp_server.h"
#include "../include/error.h"

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
uint8_t Controller::extract_control_code(vector<char> msg) {
    uint8_t code = msg[CONTROL_CODE_BYTE_POS];
    return code;
}

vector<char> Controller::extract_payload(vector<char> msg) {
   vector<char> payload;
   for(int i = HEADER_BYTE_SIZE; i < msg.size(); i++) {
        payload.push_back(msg[i]);
   }
   return payload;
}
/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
Controller::Controller() { }

Controller::~Controller() { }

void Controller::start(string control_port) {
    Router router;
    Data data;
    int control_code;

    running = true;

    // Begin listening for routing updates
    /*UDPServer update_server = UDPServer();
    int fd = update_server.start("4950");
    DEBUG(fd);
    while(1) {
        update_server.get_message();
    }*/

    // Begin listening for messages from controller
    TCPServer control_server = TCPServer();
    control_server.start(control_port);

    // Wait for either an AUTHOR or INIT command from the 
    // controller
    while(1) {
        // Handle any new controller connection
        uint32_t controller_ip = control_server.check_for_connections();
        set_ip(controller_ip);
        // Retrieve controller message
        vector<char> msg = control_server.get_message();  
        // Nothing to do if no new messages
        if(msg.empty()) {
            continue;
        }
        // Extract control code
        control_code = extract_control_code(msg);
        // Handle AUTHOR command
        if(control_code == AUTHOR) {
            vector<char> resp = generate_response(msg);
            control_server.broadcast(resp);
            continue;
        }
        // Handle INIT command
        if(control_code == INIT) {
            // Initialize routing updates and data servers.
            // TODO: data server
            vector<char> payload = extract_payload(msg);
            router.init(payload);

            // Send response to controller
            vector<char> resp = generate_response(msg);
            control_server.broadcast(resp);
            break;
        }
    }
    // At this point the router is initialized.
}

void Controller::stop() {
    running = false;
}

void Controller::set_ip(uint32_t ip) {
    controller_ip = ip;
}

vector<char> Controller::generate_response(vector<char> msg) {
    uint8_t control_code = extract_control_code(msg);
    Response response(controller_ip);
    switch(control_code) {
        case 0x00:
            // AUTHOR
            return response.author();
            break;
        case 0x01:
            // INIT
            return response.init();
            break;
        case 0x02:
            // ROUTING-TABLE
            break;
        case 0x03:
            // UPDATE
            break;
        case 0x04:
            // CRASH
            break;
        case 0x05:
            // SENDFILE
            break;
        case 0x06:
            // SENDFILE-STATS
            break;
        case 0x07:
            // LAST-DATA-PACKET
            break;
        case 0x08:
            // PENULTIMATE-DATA-PACKET
            break;
    }
}