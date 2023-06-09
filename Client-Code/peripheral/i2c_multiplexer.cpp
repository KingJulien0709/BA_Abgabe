#include "i2c_multiplexer.hpp"


void i2c_multiplexer_init(){
    Wire.begin();
}

/**
 * @brief Change the channel of the multiplexer
 * 
 * @param channel from 0-7
 */
void i2c_multiplexer_change_channel(uint8_t channel){
    i2c_write_without_register(I2C_MULTIPLEXER_ADRESS,(1<<channel));
    log_debug("i2c multiplexer channel changed to "+String(1<<channel));
}

/**
 * @brief Reset multiplexer. See 10.1 Power-On Reset Requirements on Datasheet: 1811081942_Texas-Instruments-TCA9548APWR_C130026.pdf
 * 
 */
void i2c_multiplexer_reset(uint8_t resetPin){
    //currently not needed
}