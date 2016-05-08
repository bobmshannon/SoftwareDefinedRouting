/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:41:26
* @Last Modified by:   Bobby
* @Last Modified time: 2016-05-08 01:18:17
*/

#include "../include/controller.h"
#include "../include/router.h"
#include "../include/neighbor.h"
#include <algorithm> 

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
    int control_code;

    running = true;

    // Begin listening for messages from controller
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
    // Begin listening for routing updates
    updates_server.start(router.get_router_port());
    while(1) {
        // Process any incoming control messages
        process_control_msg();    

        // Process any incoming routing updates
        process_routing_update();

        sleep(3);

        // Broadcast distance vector updates to neighbors
        broadcast_routing_updates();
    }
}

void Controller::broadcast_routing_updates() {
    vector<char> update_pkt = router.get_routing_update();
    vector<struct neighbor> neighbors = router.get_neighbors();
    for(int i = 0; i < neighbors.size(); i++) {
        udp_client.send_message(neighbors[i].ip, neighbors[i].router_port, update_pkt);
    }
}

void Controller::stop() {
    running = false;
}

void Controller::set_ip(uint32_t ip) {
    controller_ip = ip;
}

void Controller::process_routing_update() {
    vector<char> routing_update = updates_server.get_message();
    if(routing_update.empty()) {
        return;
    }

    struct routing_update_pkt update_pkt;
    std::copy(routing_update.begin(), routing_update.end(), reinterpret_cast<char*>(&update_pkt));
    std::memcpy(&update_pkt, &routing_update[0], sizeof update_pkt); // C++11 allows response.data()

    struct in_addr in;
    in.s_addr = update_pkt.source_ip;
    char *ip_cstr = inet_ntoa(in);

    DEBUG("Received routing update from: " << ip_cstr << ":" << update_pkt.source_port);
}

void Controller::process_control_msg() {
    // Check for controller messages
    uint32_t controller_ip = control_server.check_for_connections();
    set_ip(controller_ip);
    vector<char> control_msg = control_server.get_message(); 

    // Nothing to do if no message
    if(control_msg.empty()) {
        return;
    }

    // Extract payload
    vector<char> control_payload = extract_payload(control_msg);

    // Prepare a response to the control server
    vector<char> resp = generate_response(control_msg);
    uint8_t control_code = extract_control_code(control_msg);

    // Perform some actions before sending response to control server
    switch(control_code) {
        case 0x00:
            // AUTHOR
            control_server.broadcast(resp);
            break;
        case 0x01:
            // INIT
            control_server.broadcast(resp);
            break;
        case 0x02:
            // ROUTING-TABLE
            control_server.broadcast(resp);
            break;
        case 0x03:
            // UPDATE
            router.update_routing_table(control_payload);
            control_server.broadcast(resp);
            break;
        case 0x04:
            // CRASH
            control_server.broadcast(resp);
            DEBUG("simulating router crash...exiting");
            exit(0);
            break;
        case 0x05:
            // SENDFILE
            control_server.broadcast(resp);
            break;
        case 0x06:
            // SENDFILE-STATS
            control_server.broadcast(resp);
            break;
        case 0x07:
            // LAST-DATA-PACKET
            control_server.broadcast(resp);
            break;
        case 0x08:
            // PENULTIMATE-DATA-PACKET
            control_server.broadcast(resp);
            break;
    }
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
            return response.routing_table(router.get_routing_table());
            break;
        case 0x03:
            // UPDATE
            return response.update();
            break;
        case 0x04:
            // CRASH
            return response.crash();
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