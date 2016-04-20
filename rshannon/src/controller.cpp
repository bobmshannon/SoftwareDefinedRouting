/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:41:26
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-19 20:35:22
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
/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
Controller::Controller() { }

Controller::~Controller() { }

void Controller::start(string control_port) {
    running = true;

    // Begin listening for routing updates
    UDPServer update_server = UDPServer();
    int fd = update_server.start("2000");
    DEBUG(fd);
    while(1) {
        update_server.get_message();
    }

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
        // Handle AUTHOR command
        if(msg[CONTROL_CODE_BYTE_POS] == AUTHOR) {
            vector<char> resp = generate_response(msg);
            control_server.broadcast(resp);
            continue;
        }
        // Handle INIT command
        if(msg[CONTROL_CODE_BYTE_POS] == INIT) {
            // Initialize router inside here
            break;
        }
    }
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