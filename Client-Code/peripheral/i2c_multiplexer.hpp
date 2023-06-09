#pragma once
#ifndef _I2C_MULTIPLEXER_H
#define _I2C_MULTIPLEXER_H

#define I2C_MULTIPLEXER_ADRESS 0x70

#include "service/i2c.hpp"
#include "Wire.h"
#include "utils/log.hpp"


void i2c_multiplexer_init();
void i2c_multiplexer_change_channel(uint8_t channel);
void i2c_multiplexer_reset(uint8_t resetPin);


#endif