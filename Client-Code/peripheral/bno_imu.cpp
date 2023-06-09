#include "bno_imu.hpp"


Adafruit_BNO055 bno=Adafruit_BNO055(55,0x28, &Wire);
sensors_event_t event_orientation , event_acceleration;

  
void bno_imu_init(){
    i2c_multiplexer_change_channel(BNO_IC2_CHANNEL);
    if(!bno.begin(OPERATION_MODE_NDOF)){
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.println("no BNO055 detected ... Check your wiring or I2C ADDR!");
        return;
    }
    delay(100);
    bno_imu_init();
}


void bno_imu_get_sensor_data_struct_char(char* buf){
    i2c_multiplexer_change_channel(BNO_IC2_CHANNEL);
    if(!bno.begin(OPERATION_MODE_NDOF)){
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("no BNO055 detected.");
        buf[0]='\0';
        return; //return and set the given array to empty string
    }
    imu::Quaternion quat = bno.getQuat(); //get the quaternion from the imu
    bno.getEvent(&event_acceleration,Adafruit_BNO055::VECTOR_ACCELEROMETER); //get the acceleration from the imu

    double wq=quat.w();
    double xq=quat.x();
    double yq=quat.y();
    double zq=quat.z();

    char arr[128];
    snprintf(arr, sizeof(arr),"{\"w\":%1.5f,\"xq\":%1.5f,\"yq\":%1.5f,\"zq\":%1.5f,\"x\":%3.2f,\"y\":%3.2f,\"z\":%3.2f}",
    wq,xq,yq,zq,event_acceleration.acceleration.x,event_acceleration.acceleration.y,event_acceleration.acceleration.z); //format the data as json string, which can be send to the master

    log_debug(arr);
    strcpy(buf,arr);
}