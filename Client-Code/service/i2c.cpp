#include "i2c.hpp"


void i2c_init(uint8_t sd_pin,uint8_t sc_pin){
        Wire.begin(sd_pin,sc_pin);
}
/**
 * @brief 
 * 
 * @param address 7-Bit address
 * @param data_byte 
 */
void i2c_write_without_register(uint8_t address, uint8_t data_byte){
    Wire.beginTransmission(address); //address of the i2c device which should be written to
    Wire.write(data_byte); //data_byte to write to the i2c device, no register address needed
    Wire.endTransmission(); //stop transmitting
}

void i2c_write_with_register(uint8_t address, uint8_t register_address,uint8_t data_byte){
    Wire.beginTransmission(address); //address of the i2c device which should be written to
    Wire.write(register_address); //register address which should be written to
    Wire.write(data_byte); //data_byte to write to the i2c device
    Wire.endTransmission();
}

uint8_t i2c_read_without_register(uint8_t address){
    uint8_t data = 0;
    Wire.requestFrom((int)address, (int) 1); //request 1 byte from the i2c device
    while (Wire.available()) {  // slave may send less than requested
        data =(uint8_t) Wire.read();//read the data_byte from the i2c device
    }
    return data;
}

uint8_t i2c_read_with_register(uint8_t address, uint8_t register_address){
    i2c_write_without_register(address,register_address);//data is register_address, to access the register and signal the i2c device to send the data_byte
    return i2c_read_without_register(address); //read the before requested data_byte
}



