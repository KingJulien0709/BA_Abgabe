#include "force_sensor.hpp"

//sensor has a radius of 0.75 cm -> area of 1,767 cm^2

void force_sensor_init(){
    pinMode(FORCE_SENSOR_ADC_PIN, INPUT);
}

uint8_t force_sensor_read(){

    float vout = analogReadMilliVolts(FORCE_SENSOR_ADC_PIN);
    float resistance = FORCE_SENOR_RESISTANCE*(FORCE_SENOR_VOLTAGE-vout)/vout;
    if(resistance > 10000){
        resistance = 10000;
    }
    return 0;
}

//function to calculate force from resistance from a log-log plot where the x-axis is the force and the y-axis is the resistance
// 100 g has a resistance of 15 kOhm
// 1000 g has a resistance of 2.5 kOhm
//uint16_t force_sensor_get_force_from_resistance(float resistance){







