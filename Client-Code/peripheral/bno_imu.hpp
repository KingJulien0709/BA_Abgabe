#pragma once
#ifndef _BNO_IMU_HPP
#define _BNO_IMU_HPP

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BusIO_Register.h>
#include <ArduinoJson.h>
#include "utils/log.hpp"
#include "i2c_multiplexer.hpp"


#define BNO_IC2_CHANNEL 5

void bno_imu_init();
void bno_imu_get_sensor_data(int16_t *arr);
void bno_imu_get_sensor_data_struct_char(char* buf);
void bno_log_acceleration(uint8_t value);

#endif