#pragma once
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include "WiFi.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include <sys/select.h>

void communication_connect_wifi(const char* w_ssid,const char* w_pw);

void communication_create_socket_connection(uint16_t port, const char* master_ip_address);


#endif