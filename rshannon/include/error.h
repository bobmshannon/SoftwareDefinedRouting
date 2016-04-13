#ifndef _ERROR_H_
#define _ERROR_H_

#include <string>

#define ERR_INVALID_IP -1
#define ERR_CONNECTION -2
#define ERR_UNKNOWN_IP -3
#define ERR_DUPLICATE_BAN -4
#define ERR_NO_BAN -5
#define ERR_NOT_LOGGED_IN -6
#define ERR_INVALID_ARGS -7
#define ERR_SOCKET_BIND -8
#define ERR_SOCKET_LISTEN -9
#define ERR_SOCKET_INIT -10
#define ERR_SOCKET_ACCEPT -11
#define ERR_CONN_CLOSED -12
#define ERR_SOCKET_READ -13
#define ERR_PORT -14
#define ERR_INVALID_SERVER -15

#define DEBUG_MODE 1 // Whether debugging mode is enabled or disabled
#define DEBUG(x)                                                               \
  do {                                                                         \
    if (DEBUG_MODE) {                                                          \
      std::cerr << x << std::endl;            \
    }                                                                          \
  } while (0) // http://stackoverflow.com/questions/14251038/debug-macros-in-c

inline std::string err_to_str(int errcode) {
    switch (errcode) {
    case ERR_INVALID_IP:
        return "InvalidIPError";
    case ERR_CONNECTION:
        return "ConnectionError";
    case ERR_DUPLICATE_BAN:
        return "UnknownIPError";
    case ERR_NO_BAN:
        return "NoBanError";
    case ERR_INVALID_ARGS:
        return "InvalidArgumentsError";
    case ERR_SOCKET_BIND:
        return "SocketBindError";
    case ERR_SOCKET_LISTEN:
        return "SocketListenError";
    case ERR_SOCKET_INIT:
        return "SocketInitError";
    case ERR_SOCKET_ACCEPT:
        return "SocketAcceptError";
    case ERR_CONN_CLOSED:
        return "ConnectionClosedError";
    case ERR_SOCKET_READ:
        return "SocketReadError";
    case ERR_PORT:
        return "InvalidPortError";
    case ERR_INVALID_SERVER:
        return "InvalidServerError";
    default:
        return "UnknownError";
    }
}

inline bool is_err(int errcode) { return errcode < 0 ? true : false; }

#endif