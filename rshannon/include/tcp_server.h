#ifndef _SERVER_H_
#define _SERVER_H_

#define MESSAGE_SIZE 512

using std::string;
using std::vector;

class TCPServer {
  private:
    struct Connection {
        int fd;           // The file descriptor of the socket
    };
    string listen_port;
    vector<Connection> client_connections;
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
     * Send data to the client in the form of a string.
     * @param  str      The string to send
     * @param  clientfd The file descriptor corresponding to the appropriate
     * socket
     * @return          0 success, negative otherwise
     */
    int send_to_client(int clientfd, char buf[]);
    void add_connection(Connection c);
    int get_connection(int fd);
    void exit_server();
    bool is_valid_ip(string ip);

  public:
    TCPServer();
    ~TCPServer();
    int launch(string port);
};

#endif