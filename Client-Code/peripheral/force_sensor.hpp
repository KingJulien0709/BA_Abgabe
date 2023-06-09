#pragma once
#ifndef _FORCE_SENDOR_HPP
#define _FORCE_SENDOR_HPP

#include <Arduino.h>

#define FORCE_SENSOR_ADC_PIN 34
#define FORCE_SENOR_VOLTAGE 3.3
#define FORCE_SENOR_RESISTANCE 10000

void force_sensor_init();
uint8_t force_sensor_read();

#endif