/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-11 16:59:24
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include "../include/tcp_server.h"
#include "../include/error.h"

using std::string;
using std::strcpy;
using std::istringstream;
using std::istream_iterator;
using std::vector;
using std::find;
using std::sort;

void* TCPServer::get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

TCPServer::TCPServer() { client_connections = vector<Connection>(); }

TCPServer::~TCPServer() {}

void TCPServer::process_data(int sockfd, string data) {

}

int TCPServer::init_socket(string port) {
    int listener;
    int yes = 1; // for setsockopt() SO_REUSEADDR, below
    struct addrinfo hints, *ai, *p;

    // Set port
    listen_port = port;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port.c_str(), &hints, &ai) != 0) {
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
        return ERR_SOCKET_BIND;
    }

    freeaddrinfo(ai);

    // Listen for new connections on socket
    if (listen(listener, 10) == -1) {
        return ERR_SOCKET_LISTEN;
    }

    return listener;
}

int TCPServer::get_connection(int fd) {
    for (int i = 0; i < client_connections.size(); i++) {
        if (client_connections[i].fd == fd) {
            return i;
        }
    }
    return -1;
}


int TCPServer::send_to_client(int clientfd, char buf[]) {
    int total = 0;
    int bytesleft = MESSAGE_SIZE;
    int n;

    while (total < MESSAGE_SIZE) {
        n = send(clientfd, buf + total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
}

int TCPServer::new_connection_handler(int listener) {
    struct sockaddr_storage remoteaddr; // Client's IP address
    socklen_t addrlen;
    int newfd;
    string client_port;

    char welcome[MESSAGE_SIZE] = {'\0'};
    strcpy(welcome, "WELCOME");

    // Handle new connection
    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);

    if (newfd == -1) {
        return ERR_SOCKET_ACCEPT;
    }

    // Create a new entry in connection tracking table
    Connection connection = { newfd };
    add_connection(connection);
    return newfd;
}

void TCPServer::add_connection(Connection c) {
    client_connections.push_back(c);
}

int TCPServer::launch(string port) {
    fd_set master, read_fds;
    int fdmax, listener, clientfd, nbytes;
    char buf[MESSAGE_SIZE] = {'\0'};

    // Clear the master and temp sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Initialize socket that listens for new connections
    if ((listener = init_socket(port)) < 0) {
        return listener;
    }

    // Add the listener and STDIN file descriptors to the master set
    FD_SET(listener, &master);
    FD_SET(0, &master);

    // Keep track of the biggest file descriptor
    fdmax = listener;

    // Main loop
    while (1) {
        read_fds = master;

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        for (int i = 0; i <= fdmax; i++) {
            // Check whether file descriptor is ready to be read
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    // New connection received
                    if ((clientfd = new_connection_handler(listener)) == -1) {
                        // Unable to handle new connection
                    } else {
                        FD_SET(clientfd, &master); // add to master set
                        if (clientfd > fdmax) {    // keep track of the max
                            fdmax = clientfd;
                        }
                    }
                } else {
                    // Data received from existing connection
                    if ((nbytes = recv(i, buf, MESSAGE_SIZE, 0)) <= 0) {
                        if (nbytes == 0) {
                            // Connection closed by client
                        } else {
                            // read() error
                        }
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        // printf("received %i bytes from fd %i: %s", nbytes, i,
                        //       buf);
                        process_data(i, string(buf));
                    }
                }
            }
        }
    }

    return 0;
}
