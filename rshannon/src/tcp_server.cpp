/*
* @Author: Robert Shannon <rshannon@buffalo.edu>
* @Date:   2016-02-05 21:26:31
* @Last Modified by:   Bobby
* @Last Modified time: 2016-04-21 16:14:58
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
#include "../include/control_packet.h"

/////////////////////////////////////////////////////////////////////////////////
// PRIVATE
/////////////////////////////////////////////////////////////////////////////////
int TCPServer::init_socket(string port) {
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

int TCPServer::extract_length(char header[]) {
    /* Assumes 2 byte length field */
    char upper = header[LENGTH_PREFIX_BYTE_POS];
    char lower = header[LENGTH_PREFIX_BYTE_POS+1];
    return (upper << 8) | lower;
}

void TCPServer::close_connection(int fd) {
    for(int i = 0; i < connections.size(); i++) {
        if(connections[i].fd == fd) {
            close(fd);
            FD_CLR(fd, &master);
            connections.erase(connections.begin()+i);    
            break;        
        }
    }
}

vector<char> TCPServer::read_data(int fd) {
    DEBUG("new data arrived on fd: " << fd);
    // First read the message header
    char header[HEADER_BYTE_SIZE];
    int nbytes = 0;
    while(nbytes < HEADER_BYTE_SIZE) {
        int bytes_read = recv(fd, header, HEADER_BYTE_SIZE, 0);
        if(bytes_read <= 0) {
            DEBUG("client disconnected");
            close_connection(fd);
            return vector<char>();
        }
        nbytes += bytes_read;
    }
    DEBUG("read " << nbytes << " bytes from fd " << fd);
    nbytes = 0;
    DEBUG("received message header: " << string(header) + '\0');
    // Then the message payload
    uint16_t payload_len = extract_length(header);
    if(payload_len == 0) {
        DEBUG("no payload provided, delivering message...");
        return build_message(header);
    }
    char payload[payload_len];
    DEBUG("expecting payload of length " << payload_len << " bytes");
    while(nbytes < payload_len) {
        int bytes_read = recv(fd, payload, payload_len, 0);
        if(bytes_read < 0) {
            DEBUG("error receiving message payload: " << bytes_read);
            close(fd);
            return vector<char>();
        }
        if(bytes_read == 0) {
            DEBUG("client disconnected");
            close(fd);
            return vector<char>();
        }
        nbytes += bytes_read;
    }
    DEBUG("received payload");
    // Now combine both the header and payload into a single message...
    DEBUG("delivering message");
    return build_message(header, payload, payload_len);
}

vector<char> TCPServer::build_message(char header[]) {
    vector<char> message;
    for(int i = 0; i < HEADER_BYTE_SIZE; i++) {
        message.push_back(header[i]);
    }
    return message;   
}

vector<char> TCPServer::build_message(char header[], char payload[], int payload_len) {
    vector<char> message;
    for(int i = 0; i < HEADER_BYTE_SIZE; i++) {
        message.push_back(header[i]);
    }
    for(int i = 0; i < payload_len; i++) {
        message.push_back(payload[i]);
    }
    return message;   
}

int TCPServer::new_connection_handler(int listener) {
    DEBUG("handling new connection...");
    struct sockaddr_in remoteaddr; // Client's IP address
    socklen_t addrlen;
    int newfd;

    // Handle new connection
    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);

    if (newfd == -1) {
        DEBUG(err_to_str(ERR_SOCKET_ACCEPT));
        return ERR_SOCKET_ACCEPT;
    }

    // Track connection
    struct connection connection;
    connection.fd = newfd;
    connection.ip = ntohl(remoteaddr.sin_addr.s_addr);

    connections.push_back(connection);

    DEBUG("new fd created: " << newfd);
    return newfd;
}

int TCPServer::send_to_client(int clientfd, vector<char> msg) {
    char buf[msg.size()];
    int total = 0;
    int bytesleft = msg.size();
    int n;

    for(int i = 0; i < msg.size(); i++) {
        buf[i] = msg[i];
    }

    DEBUG("sending message to fd " << clientfd);

    while (bytesleft > 0) {
        n = send(clientfd, buf + total, bytesleft, 0);
        if (n == -1) {
            DEBUG("error sending message to fd " << clientfd);
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
}

/////////////////////////////////////////////////////////////////////////////////
// PUBLIC
/////////////////////////////////////////////////////////////////////////////////
TCPServer::TCPServer() {
    listening = false;
    fdmax = 0, listener = 0;
}

TCPServer::~TCPServer() { }

int TCPServer::start(string port) {
    return init_socket(port);
}

uint32_t TCPServer::last_known_client_ip() {
    int num_conn = connections.size();
    if(num_conn == 0) {
        return 0;
    }
    return connections[num_conn-1].ip;
}

uint32_t TCPServer::check_for_connections() {
    int clientfd;
    struct timeval tv;

    // Make sure server is listening for new connections
    if(!listening) {
        DEBUG("not listening, can't check for connections");
        return last_known_client_ip();
    }

    read_fds = master;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int num_set = select(fdmax + 1, &read_fds, NULL, 0, &tv);
    if(num_set == 0) {
        return last_known_client_ip();
    } else if(num_set == -1) {
        DEBUG("select error");
        exit(4);       
    }

    if (FD_ISSET(listener, &read_fds)) {
        DEBUG("listener socket ready to be read");
        // New connection received
        if ((clientfd = new_connection_handler(listener)) == -1) {
            // Unable to handle new connection
            DEBUG("unable to handle new connection");
        } else {
            FD_SET(clientfd, &master); // add to master set
            if (clientfd > fdmax) {    // keep track of the max
                fdmax = clientfd;
            }
            DEBUG("client connection accepted");
        }
    }
    return last_known_client_ip();
}

vector<char> TCPServer::get_message() {
    struct timeval tv;

    read_fds = master;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int num_set = select(fdmax + 1, &read_fds, NULL, 0, &tv);
    if(num_set == 0) {
        //DEBUG("select: no fds ready to be read");
        return vector<char>();
    } else if(num_set == -1) {
        DEBUG("select error");
        exit(4);       
    }

    if (FD_ISSET(listener, &read_fds)) {
        // Don't read data from listener socket, only for sockets
        // created for connected clients
        return vector<char>();
    }

    for(int i = 3; i <= fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) {
            return read_data(i);
        }
    }
    return vector<char>();
}

void TCPServer::broadcast(vector<char> data) {
    for(int i = 0; i < connections.size(); i++) {
        send_to_client(connections[i].fd, data);
    }
}