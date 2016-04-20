/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-19 21:20:25
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include "../include/udp_server.h"
#include "../include/error.h"

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
int UDPServer::init_socket(string port) {
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((listener = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
            close(listener);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        DEBUG("listener: failed to bind socket");
        return 2;
    }

    freeaddrinfo(servinfo);

    // Now listening for new connections
    listening = true;

    // Clear the master and temp sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Add the listener file descriptor to the master set
    FD_SET(listener, &master);

    return listener;
}

vector<char> UDPServer::read_data(int fd) {
    socklen_t fromlen;
    struct sockaddr_storage addr;

    fromlen = sizeof addr;
    char message[MAX_DATAGRAM_SIZE];
    DEBUG("new data arrived on fd: " << fd);
    int bytes_read = recvfrom(fd, message, MAX_DATAGRAM_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
    DEBUG("read " << bytes_read << " bytes from fd " << fd);
    DEBUG("received message");
    // Now combine both the header and payload into a single message...
    DEBUG("delivering message");
    vector<char> ret;
    for(int i = 0; i < bytes_read; i++) {
        ret.push_back(message[i]);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
UDPServer::UDPServer() {
    listening = false;
    listener = 0;
}

UDPServer::~UDPServer() { }

int UDPServer::start(string port) {
    return init_socket(port);
}

vector<char> UDPServer::get_message() {
    struct timeval tv;

    read_fds = master;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int num_set = select(listener + 1, &read_fds, NULL, 0, &tv);
    if(num_set == 0) {
        //DEBUG("no fds ready to be read");
        return vector<char>();
    } else if(num_set == -1) {
        DEBUG("select error");
        exit(4);       
    }

    if (FD_ISSET(listener, &read_fds)) {
        DEBUG("fd " << listener << " ready to be read");
        return read_data(listener);
    }

    return vector<char>();
}