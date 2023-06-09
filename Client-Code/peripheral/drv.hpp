#pragma once
#ifndef _DRV_H
#define _DRV_H

#include "stdint.h"
#include "i2c_multiplexer.hpp"
#include "service/i2c.hpp"
#include "Wire.h"
#include "utils/log.hpp"

#define DRV_DEVICE_COUNTER 5
#define DRV_ENABLE_PIN 19  
#define DRV_STATE_ACTIVE 0
#define DRV_STATE_STANDBY 1
#define DRV_STATE_SHUTDOWN 2

#define DRV_STATE_TRANSITION_DEV_RESET_AND_STANDBY 3
#define DRV_STATE_TRANSITION_DEV_TOGGLE_STANDBY 4

#define REALTIME_MODE 5
#define REALTIME_VALUE_REG 0x02

#define DRV_ADDR 0x5A

#define DRV_ERM_MODE 0
#define DRV_LRA_MODE 1
#define DRV_MOTOR_MODE_REG 0x1A

#define DRV_LRA_FREQUENCY_HZ 160  //or 320 depending on vibration axis
#define DRV_LRA_FREQUENCY_REG 0x20

#define DRV_RATED_VOLTAGE 2.33
#define DRV_RATED_VOLTAGE_REG 0x17


void drv_init();
void drv_single_init(uint8_t channel);
void drv_power_state_machine(uint8_t var);
void drv_run_realtime(uint8_t chanel, uint8_t level);
void drv_set_realtime_mode();
uint8_t drv_get_power_state();
void drv_set_erm_lra_mode(uint8_t mode);
void drv_set_bit_in_register(uint8_t register_v,uint8_t bit_num,uint8_t value);
void drv_set_bit_in_register_for_all_devices(uint8_t register_v,uint8_t bit_num,uint8_t value);
void drv_set_lra_frequency(void);
void drv_set_rated_voltage(void);
#endif