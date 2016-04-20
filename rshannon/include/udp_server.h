#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

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

#define MAX_DATAGRAM_SIZE 65535

class UDPServer {
  private:
    string listen_port;
    fd_set master, read_fds;
    int listener;
    bool listening;
    /**
     * Initialize a new socket on specified port
     * 
     * @param  port The port to listen on
     * @return      A valid file descriptor > 0, negative otherwise
     */
    int init_socket(string port);
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

  public:
    UDPServer();
    ~UDPServer();

    /**
     * Start listening for new connections on the specified port
     * 
     * @param  port the port to listen on
     * @return      the file desriptor of the listening socket
     */
    int start(string port);
    /**
     * Check for a new message and return it
     *
     * @return empty vector<char> if no new message, otherwise a
     * vector<char> with the message
     */
    struct vector<char> get_message();
};

#endif