#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#define MESSAGE_SIZE 512

using std::string;
using std::vector;

class TCPClient {
  private:
    int sockfd;         // File descriptor for network socket
    fd_set master, read_fds;

    void* get_in_addr(struct sockaddr* sa);
    /**
     * Initialize a socket for communication with specified
     * remote host and port.
     *
     * @param  host The hostname of the server
     * @param  port The port to connect to
     * @return      0 if sucessful, negative integer otherwise
     */
    int server_connect(string host, string port);
    /**
     * Close the socket (if currently logged in to a
     * remote server, i.e. a socket exists)
     * @return 0 if sucessful, negative integer otherwise
     */
    int server_disconnect();

    int is_valid_ip(string ip);

    /**
     * Login to the server located at ip address: <server-ip> listening on port:
     * <server-port>.
     *
     * @param  host The hostname of the server
     * @param  port The port to connect to
     * @return      0 if sucessful, negative integer otherwise
     */
    void login(string host, string port);

    /**
     * Send data to the server in the form of a string
     * @param  str The string to send
     * @return     0 on success, negative otherwise
     */
    int send_to_server(string str);

  public:
    TCPClient();
    ~TCPClient();
    /**
     * Process a user inputted command.
     * @param cmd The user inputted string
     */
    void process_command(string cmd);
    /**
     * Launch a new client listening on specified port.
     */
    void launch(string port);
    /**
     * Exit client window.
     */
    void exit_client();
};

#endif