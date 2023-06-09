#pragma once
#ifndef _BATTERY_MANAGEMENT_HPP
#define _BATTERY_MANAGEMENT_HPP

#define BATTERY_MANAGEMENT_ENABLE_PIN_1 26
#define BATTERY_MANAGEMENT_ENABLE_PIN_2 25


#include "Arduino.h"

void battery_management_set_fast_charging();//1.5A
void battery_management_set_normal_charging();//500mA
void battery_management_set_slow_charging();//100mA

#endif