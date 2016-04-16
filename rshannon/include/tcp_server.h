#ifndef _SERVER_H_
#define _SERVER_H_

#define MESSAGE_SIZE 512

#include <string>
#include <vector>
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

using std::string;
using std::vector;
using std::string;
using std::strcpy;
using std::istringstream;
using std::istream_iterator;
using std::vector;
using std::find;
using std::sort;

class TCPServer {
  private:
    struct connection {
        int fd;
        uint32_t ip;
    };
    string listen_port;
    fd_set master, read_fds;
    int fdmax, listener;
    bool listening;
    vector<struct connection> connections;
    /**
     * Initialize a new socket on specified port
     * 
     * @param  port The port to listen on
     * @return      A valid file descriptor > 0, negative otherwise
     */
    int init_socket(string port);
    /**
     * Handle a new connection from a client by initializing a new socket
     * 
     * @param  listener The file descriptor corresponding to the socket
     * listening for new connections
     * @return          A valid file descriptor > 0, negative otherwise
     */
    int new_connection_handler(int listener);
    /**
     * Read incoming data from a socket. This function will read
     * a single "message" with a variable payload, by first reading 
     * a pre-set header size and determining the payload size using
     * a payload length field in the header.
     * 
     * @param sockfd The file descriptor of the socket to read
     * @return vector<char> the data read from the socket
     */
    vector<char> read_data(int clientfd);
    int extract_length(char header[]);
    vector<char> build_message(char header[], char payload[], int payload_len);
    vector<char> build_message(char header[]);
    uint32_t last_known_client_ip();
    void close_connection(int fd);

  public:
    TCPServer();
    ~TCPServer();
    /**
     * Send data to the client in the form of a array of chars
     * 
     * @param  str      The string to send
     * @param  clientfd The file descriptor corresponding to the appropriate
     * socket
     * @return          0 success, negative otherwise
     */
    int send_to_client(int clientfd, vector<char> data);
    /**
     * Start listening for new connections on the specified port
     * 
     * @param  port the port to listen on
     * @return      the file desriptor of the listening socket
     */
    int start(string port);
    /**
     * Broadcast data to all connected clients
     * 
     * @param  data the data to broadcast to all clients
     */
    void broadcast(vector<char> data);
    /**
     * Check for new connections and handle them
     */
    uint32_t check_for_connections();
    /**
     * Check for a new message and return it
     *
     * @return empty vector<char> if no new message, otherwise a
     * vector<char> with the message
     */
    struct vector<char> get_message();
};

#endif