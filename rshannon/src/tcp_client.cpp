/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:41:26
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-11 17:02:13
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include <vector>
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/tcp_client.h"
#include "../include/error.h"

using std::string;
using std::istringstream;
using std::istream_iterator;
using std::vector;
using std::find;

TCPClient::TCPClient() {
    sockfd = -1;
}

TCPClient::~TCPClient() {}

void* TCPClient::get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int TCPClient::is_valid_ip(string ip) {
    struct sockaddr_in sa;

    // Check if valid IP address.
    if (inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) <= 0) {
        return -1;
    } else {
        return 0;
    }
}

int TCPClient::send_to_server(string str) {
    char buf[MESSAGE_SIZE] = {'\0'};
    for (int i = 0; i < str.length(); i++) {
        buf[i] = str[i];
        if (i == str.length() - 1) {
            buf[i + 1] = '\n';
        }
    }
    int total = 0;
    int bytesleft = MESSAGE_SIZE;
    int n;

    while (total < MESSAGE_SIZE) {
        n = send(sockfd, buf + total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
}

int TCPClient::server_connect(string host, string port) {
    int sockfd;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        return ERR_CONNECTION;
    }

    // Loop through all the results and connect to the first we can
    while (servinfo != NULL) {
        if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                             servinfo->ai_protocol)) == -1) {
            servinfo = servinfo->ai_next;
            continue;
        } else if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) ==
                   -1) {
            close(sockfd);
            servinfo = servinfo->ai_next;
            continue;
        }

        break;
    }

    if (servinfo == NULL) {
        return ERR_CONNECTION;
    }

    inet_ntop(servinfo->ai_family,
              get_in_addr((struct sockaddr*)servinfo->ai_addr), s, sizeof s);

    freeaddrinfo(servinfo);

    return sockfd;
}

int TCPClient::server_disconnect() {
    close(sockfd);
    return 0;
}

void TCPClient::exit_client() {
    exit(0);
}

void TCPClient::launch(string port) {
    int fdmax, nbytes;
    char buf[MESSAGE_SIZE + 1] = {'\0'};

    // Keep track of the biggest file descriptor
    fdmax = sockfd;

    // Main loop
    while (1) {
        read_fds = master;

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
        }

        for (int i = 0; i <= fdmax; i++) {
            // Check whether file descriptor is ready to be read
            if (FD_ISSET(i, &read_fds)) {
                if (i == sockfd) {
                    // Message received from server
                    if ((nbytes = recv(i, buf, MESSAGE_SIZE, 0)) <= 0) {
                        if (nbytes == 0) {
                            // Connection closed by client
                        } else {
                            // read() error
                        }
                        close(i);
                        FD_CLR(i, &master);
                        exit(-1);
                    } else {
                        // read data from socket (buf)
                    }
                }
            }
        }
    }
}