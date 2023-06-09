#include "log.hpp"

uint8_t _log_level;

void log_init(uint8_t log_level){
    _log_level = log_level;
    Serial.begin(115200);

    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println("\n");
}

void log_set_level(uint8_t log_level){
    _log_level = log_level;
}

void log_error(const char* msg){
    if(_log_level>=LOG_LEVEL_ERROR){
        Serial.println(msg);
    }
}
void log_error(String msg){
    if(_log_level>=LOG_LEVEL_ERROR){
        Serial.println(msg);
    }
}


void log_info(const char* msg){
    if(_log_level>=LOG_LEVEL_INFO){
        Serial.println(msg);
    }
}
void log_info(String msg){
    if(_log_level>=LOG_LEVEL_INFO){
        Serial.println(msg);
    }
}

void log_debug(const char* msg){
    if(_log_level>=LOG_LEVEL_DEBUG){
        Serial.println(msg);
    }
}
void log_debug(String msg){
    if(_log_level>=LOG_LEVEL_DEBUG){
        Serial.println(msg);
    }
}

void log_trace(const char* msg){
    if(_log_level>=LOG_LEVEL_TRACE){
        Serial.println(msg);
    }
}
void log_trace(String msg){
    if(_log_level>=LOG_LEVEL_TRACE){
        Serial.println(msg);
    }
}
