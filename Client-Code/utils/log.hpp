#pragma once
#ifndef _LOG_HPP
#define _LOG_HPP

#include "Arduino.h"

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_TRACE 4

void log_init(uint8_t log_level = LOG_LEVEL_DEBUG);
void log_set_level(uint8_t log_level);
void log_error(const char* msg);
void log_error(String msg);
void log_info(const char* msg);
void log_info(String msg);
void log_debug(const char* msg);
void log_debug(String msg);
void log_trace(const char* msg);
void log_trace(String msg);

#endif