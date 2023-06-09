#include "drv.hpp"

uint8_t state = DRV_STATE_STANDBY;


/**
 * @brief inits all drv devices
 * 
 */
void drv_init(){
    drv_power_state_machine(DRV_STATE_STANDBY);
    for(uint8_t i=0;i<DRV_DEVICE_COUNTER;i++){
        drv_single_init(i);
    }
    drv_power_state_machine(DRV_STATE_ACTIVE);
}

/**
 * @brief inits a single drv device at the given multiplxer channel
 * 
 * @param channel given multiplexer-channel starting from 0 to DRV_DEVICE_COUNTER-1
 */
void drv_single_init(uint8_t channel){
    i2c_multiplexer_change_channel(channel);
    drv_set_erm_lra_mode(DRV_LRA_MODE);
    drv_set_realtime_mode();
    drv_set_bit_in_register(0x1D,3,1); //set rtp format to unsigned 8 bit
    drv_set_lra_frequency();
    drv_set_rated_voltage();
}


/**
 * @brief sets a specific bit in a register for all devices
 * 
 * @param register_v   register to set the bit in
 * @param bit_num   bit to set in the register from 0-7
 * @param value     value to set the bit to 0 or 1
 */
void drv_set_bit_in_register_for_all_devices(uint8_t register_v,uint8_t bit_num,uint8_t value){
    for(uint8_t i=0;i<DRV_DEVICE_COUNTER;i++){
        i2c_multiplexer_change_channel(i);
        drv_set_bit_in_register(register_v,bit_num,value);
    }
}

/**
 * @brief sets a specific bit in a register for one device
 * 
 * @param register_v   register to set the bit in
 * @param bit_num   bit to set in the register from 0-7
 * @param value     value to set the bit to 0 or 1
 */
void drv_set_bit_in_register(uint8_t register_v,uint8_t bit_num,uint8_t value){
    uint8_t register_value = i2c_read_with_register(DRV_ADDR,register_v);
    if(value==1){
        register_value |= 1<<bit_num;
    }else{
        register_value &= ~(1<<bit_num);
    }
    i2c_write_with_register(DRV_ADDR, register_v, register_value);
}


/**
 * @brief state machine for the graph 8.4 on the datasheet https://www.ti.com/lit/ds/symlink/drv2605l.pdf?ts=1669975217122&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FDRV2605L
 *      the state machine is used to set the state of all the drv devices
 * @param var The state to set the controller. Use the defines DRV_STATE_ACTIVE, DRV_STATE_STANDBY, DRV_STATE_SHUTDOWN, DRV_STATE_TRANSITION_DEV_RESET_AND_STANDBY, DRV_STATE_TRANSITION_DEV_TOGGLE_STANDBY
 */
void drv_power_state_machine(uint8_t var){
    if(state==DRV_STATE_ACTIVE){
        if(var==DRV_STATE_SHUTDOWN){
            digitalWrite(DRV_ENABLE_PIN,0); //disable the drv by setting the enable pin to LOW
            state=DRV_STATE_SHUTDOWN;
        }else if(var==DRV_STATE_TRANSITION_DEV_RESET_AND_STANDBY){
            drv_set_bit_in_register_for_all_devices(0x01,7,1);//set bit for devreset in register to 1 TODO change to just change the bit 7 
            state=DRV_STATE_STANDBY;
        }else if(var==DRV_STATE_TRANSITION_DEV_TOGGLE_STANDBY){
            drv_set_bit_in_register_for_all_devices(0x01,6,1);//set bit for standby in register to 1 TODO change to just change the bit 6 
            state=DRV_STATE_STANDBY;
        }
    }else if(state==DRV_STATE_STANDBY){
        if(var==DRV_STATE_SHUTDOWN){
            digitalWrite(DRV_ENABLE_PIN,0); //disable the drv by setting the enable pin to LOW
            state=DRV_STATE_SHUTDOWN;
        }else if(var==DRV_STATE_ACTIVE){
            drv_set_bit_in_register_for_all_devices(0x01,6,0);//set bit for standby in register to 0  TODO change to just change the bit 6
            state=DRV_STATE_ACTIVE;
        }
    }else if(state==DRV_STATE_SHUTDOWN){
        if(var==DRV_STATE_STANDBY){
            digitalWrite(DRV_ENABLE_PIN,1); //enable the drv by setting the enable pin to HIGH
            state=DRV_STATE_STANDBY;
        }
    }
}

/**
 * @brief sets the drv to the realtime mode 
 * 
 * @return * set 
 */
void drv_set_realtime_mode(){
    i2c_write_with_register(DRV_ADDR,0x01,REALTIME_MODE); //dont care about the other bits in 0x01, because they have to be zero in when in realtime mode
}

/**
 * @brief play a realtime value for the given channel
 * 
 * @param channel given multiplexer-channel starting from 0 to DRV_DEVICE_COUNTER-1
 * @return * set 
 */
void drv_run_realtime(uint8_t chanel, uint8_t level){
    i2c_multiplexer_change_channel(chanel);
    i2c_write_with_register(DRV_ADDR,REALTIME_VALUE_REG,level);
}

/**
 * @brief sets the kind of motor for drv
 * @param mode 0(DRV_ERM_MODE) for ERM and 1(DRV_LRA_MODE) for LRA
 * @return * set 
 */
void drv_set_erm_lra_mode(uint8_t mode){
    uint8_t register_value = i2c_read_with_register(DRV_ADDR,DRV_MOTOR_MODE_REG);
    if(mode==DRV_ERM_MODE){
        register_value &= (uint8_t) ~(1<<7); //set bit 7 to 0 to set to erm motor mode
    }else if(mode==DRV_LRA_MODE){
        register_value |= (uint8_t) (1<<7);  //set bit 7 to 1 to set to lra motor mode
    }
    i2c_write_with_register(DRV_ADDR,DRV_MOTOR_MODE_REG,register_value);
    
}

/**
 * @brief sets the lra frequency for the drv 
 * the value as float is calculated by the formula: 1/(f*98.49*10^-6) where f is the resonant frequency of the lra in Hz.
 * the value as byte is calculated by rounding the value as float to the nearest unsigned uint8_t value
 */
void drv_set_lra_frequency(void){
    float ol_lra_perioad = (1/(DRV_LRA_FREQUENCY_HZ*98.49*pow10(-6))); //formula from datasheet
    log_debug("Set frequency(float) to "+String(ol_lra_perioad));
    uint8_t ol_lra_perioad_uint8 = (uint8_t) floorf(ol_lra_perioad+0.5); //round up to the nearest uint8_t
    log_debug("Set frequency(byte) to "+String(ol_lra_perioad_uint8));
    i2c_write_with_register(DRV_ADDR,DRV_LRA_FREQUENCY_REG,ol_lra_perioad_uint8);
}

void drv_set_rated_voltage(void){
    float od_clamp = DRV_RATED_VOLTAGE/(0,2132*sqrt(1-DRV_LRA_FREQUENCY_HZ*0.0008)); //formula from datasheet
    log_debug("Set rated voltage(float) to "+String(od_clamp));
    uint8_t od_clamp_uint8 = (uint8_t) floorf(od_clamp); //round to the nearest uint8_t
    log_debug("Set rated voltage(byte) to "+String(od_clamp_uint8));
    i2c_write_with_register(DRV_ADDR,DRV_RATED_VOLTAGE_REG,od_clamp_uint8);
}

uint8_t drv_get_power_state(){
    return state;
}