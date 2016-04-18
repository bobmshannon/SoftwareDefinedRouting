/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-18 00:04:47
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
    int yes = 1; // for setsockopt() SO_REUSEADDR, below
    struct addrinfo hints, *ai, *p;

    // Set port
    listen_port = port;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port.c_str(), &hints, &ai) != 0) {
        DEBUG(err_to_str(ERR_SOCKET_INIT));
        return ERR_SOCKET_INIT;
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    if (p == NULL) {
        std::cerr << err_to_str(ERR_SOCKET_BIND);
        return ERR_SOCKET_BIND;
    }

    freeaddrinfo(ai);

    // Listen for new connections on socket
    if (listen(listener, 10) == -1) {
        DEBUG(err_to_str(ERR_SOCKET_LISTEN));
        return ERR_SOCKET_LISTEN;
    }

    // Now listening for new connections
    listening = true;

    // Clear the master and temp sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Add the listener ile descriptors to the master set
    FD_SET(listener, &master);

    // Keep track of the biggest file descriptor
    fdmax = listener;

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
    fdmax = 0, listener = 0;
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

    int num_set = select(fdmax + 1, &read_fds, NULL, 0, &tv);
    if(num_set == 0) {
        return vector<char>();
    } else if(num_set == -1) {
        DEBUG("select error");
        exit(4);       
    }

    for(int i = 3; i <= fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) {
            return read_data(i);
        }
    }
    return vector<char>();
}