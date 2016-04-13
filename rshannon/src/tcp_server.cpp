/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-13 00:25:27
*
* Note that some of the networking code used in this file
* was directly taken from the infamous Beej Network Programming
* Guide: http://beej.us/guide/bgnet/. This includes the example
* code snippets which demonstrate how to monitor each file descriptor
* using select(), and setting up a basic client and server model with
* sockets.
*/

#include "../include/tcp_server.h"
#include "../include/error.h"

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
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

int TCPServer::extract_length(char header[]) {
    /* Assumes 2 byte length field */
    char upper = header[length_prefix_byte_pos];
    char lower = header[length_prefix_byte_pos+1];
    return ntohl(upper + (lower >> sizeof(char)));
}

vector<char> TCPServer::read_data(int fd) {
    // First read the message header
    char header[header_byte_size];
    int nbytes = recv(fd, header, header_byte_size, 0);
    if(nbytes <= 0) {
        return vector<char>();
    }
    // Then the message payload
    int payload_len = extract_length(header);
    char payload[payload_len];
    nbytes = recv(fd, payload, payload_len, 0);
    if(nbytes <= 0) {
        return vector<char>();
    }
    // Now combine both the header and payload into a single message...
    string data = string(header) + string(payload);
    // Return the message in vector<char> form
    vector<char> message;
    for(int i = 0; i < data.size(); i++) {
        message.push_back(data[i]);
    }
    return message;
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

    return newfd;
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
TCPServer::TCPServer() {
    listening = false;
    fdmax = 0, listener = 0, clientfd = 0, nbytes = 0;
    header_byte_size = 8, length_prefix_byte_pos = 6, num_bytes_length_prefix = 2;
}

TCPServer::~TCPServer() { }

int TCPServer::start(string port) {
    return init_socket(port);
}

void TCPServer::check_for_connections() {
    // Make sure server is listening for new connections
    if(!listening) {
        return;
    }

    read_fds = master;

    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
        perror("select");
        exit(4);
    }

    if (FD_ISSET(listener, &read_fds)) {
        // New connection received
        if ((clientfd = new_connection_handler(listener)) == -1) {
            // Unable to handle new connection
        } else {
            FD_SET(clientfd, &master); // add to master set
            if (clientfd > fdmax) {    // keep track of the max
                fdmax = clientfd;
            }
        }
    } 
}

vector<char> TCPServer::get_message() {
    for(int i = 0; i < fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) {
            // message received from controller...
        }
    }
    return vector<char>();
}


/*
int TCPServer::launch(string port) {
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
                    /*if ((nbytes = recv(i, buf, MESSAGE_SIZE, 0)) <= 0) {
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
}*/
