#ifndef _TCP_SOCKET_COMMUNICATION_H
#define _TCP_SOCKET_COMMUNICATION_H

#include "Arduino.h"
#include "stdint.h"
#include "lwip/sockets.h"
#include "utils/log.hpp"

#define TCP_SOCKET_READ 0
#define TCP_SOCKET_WRITE 1

#define TCP_SOCKET_READ_SIZE 64
#define TCP_SOCKET_SEND_SIZE 128

class TCP_Socket_Communication{
    private:
        bool blocking_mode_;
        int port_;
        const char* ip_address_;
        int socket_num_;
        struct sockaddr_in sock_struct_local_; 
    public:
        TCP_Socket_Communication(int port, const char* ip_address);
        void tcp_socket_init();
        int tcp_socket_connect();
        int tcp_socket_close();
        int tcp_socket_send_string(char *data,uint8_t len);
        char* tcp_socket_receive_string_blocking();
        char* tcp_socket_receive_string_non_blocking();
        void tcp_socket_configure_block_mode(bool block_mode);
        void tcp_socket_reconnect();
        void tcp_socket_reinit();
};

#endif