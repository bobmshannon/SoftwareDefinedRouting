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
    string listen_port;
    fd_set master, read_fds;
    int fdmax, listener, clientfd, nbytes;
    bool listening;

    void* get_in_addr(struct sockaddr* sa);
    /**
     * Initialize a new socket on specified port
     * @param  port The port to listen on
     * @return      A valid file descriptor > 0, negative otherwise
     */
    int init_socket(string port);
    /**
     * Handle a new connection from a client by initializing a new socket.
     * @param  listener The file descriptor corresponding to the socket
     * listening for new connections
     * @return          A valid file descriptor > 0, negative otherwise
     */
    int new_connection_handler(int listener);
    /**
     * Processing incoming data string from client.
     * @param sockfd The file descriptor the data was read from
     * @param data   The data that was read
     */
    void process_data(int sockfd, string data);
    /**
     * Send data to the client in the form of a array of chars.
     * @param  str      The string to send
     * @param  clientfd The file descriptor corresponding to the appropriate
     * socket
     * @return          0 success, negative otherwise
     */
    int send_to_client(int clientfd, vector<char> data);

  public:
    TCPServer();
    ~TCPServer();
    int start(string port);
    int broadcast(vector<char> data);
    void check_for_connections();
    struct vector<char> get_message();
};

#endif