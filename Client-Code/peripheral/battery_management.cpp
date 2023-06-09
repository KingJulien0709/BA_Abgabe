#include "battery_management.hpp"

void battery_management_set_fast_charging(){ //provides 1.41A input current
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_1,LOW);
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_2,HIGH);
}

void battery_management_set_normal_charging(){ //provides 500mA input current
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_1,HIGH);
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_2,LOW);
}

void battery_management_set_slow_charging(){ //provides 100mA input current
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_1,LOW);
    digitalWrite(BATTERY_MANAGEMENT_ENABLE_PIN_2,LOW);
}