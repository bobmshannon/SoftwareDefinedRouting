/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-05-07 22:40:45
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include "../include/udp_client.h"
#include "../include/error.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <cstring>

using std::ostringstream;
using std::string;

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
void UDPClient::send_data(uint32_t ip, uint16_t port, int nbytes, char buf[]) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    struct in_addr in;
    in.s_addr = ip;
    char *ip_cstr = inet_ntoa(in);

    ostringstream ss;
    ss << port;
    string port_string = ss.str();
    const char *port_cstr = port_string.c_str();

    if ((rv = getaddrinfo(ip_cstr, port_cstr, &hints, &servinfo)) != 0) {
        DEBUG("getaddrinfo: %s\n" << gai_strerror(rv));
        return;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            DEBUG("udp client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        DEBUG("udp client: failed to create socket\n");
        return;
    }

    if ((numbytes = sendto(sockfd, buf, nbytes, MSG_DONTWAIT,
             p->ai_addr, p->ai_addrlen)) == -1) {
        DEBUG("udp client: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    DEBUG("udp client: sent " << nbytes << " bytes to " << ip_cstr);
    close(sockfd);
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
UDPClient::UDPClient() { }

UDPClient::~UDPClient() { }

void UDPClient::send_message(uint32_t ip, uint16_t port, vector<char> msg) {
    int nbytes = sizeof(msg);
    char buf[nbytes];
    memcpy(buf, &msg, nbytes);
    send_data(ip, port, nbytes, buf);
}