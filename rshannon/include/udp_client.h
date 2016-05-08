#ifndef _UDPCLIENT_H_
#define _UDPCLIENT_H_

#define MAX_DATAGRAM_SIZE 65535

#include <vector>
#include <stdint.h>

using std::vector;

class UDPClient {
  private:
    void send_data(uint32_t ip, uint16_t port, int nbytes, char buf[]);

  public:
    UDPClient();
    ~UDPClient();

    /**
     * Send a UDP datagram to a destination host
     */
    void send_message(uint32_t ip, uint16_t port, vector<char> msg);
};

#endif