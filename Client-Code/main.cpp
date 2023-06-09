#include <Arduino.h>
#include "WiFi.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "peripheral/i2c_multiplexer.hpp"
#include "service/communication.hpp"
#include "classes/tcp_socket_communication.hpp"
#include "peripheral/bno_imu.hpp"
#include <ArduinoJson.h>
#include "utils/log.hpp"
#include "peripheral/drv.hpp"
#include "AsyncElegantOTA.h"
#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include "peripheral/battery_management.hpp"
#include "freertos/semphr.h"
#include "FreeRTOS.h"
#include "freertos/task.h"


#define UPDATEINTERVAL_MS 20
#define ENABLE_MULTITASKING 1 //set to 1 to enable multitasking else run in super-loop

#define WIFI_SSID "Debug Network"
#define WIFI_PASSWORT "1234567890"

#define HOST_IP_ADDR "192.168.43.226"//"192.168.178.31"//"192.168.178.44" //ip of the tcp master device
#define PORT 5000 //port of the tcp socket

//#define PIN_SDA currently not needed, because default pin is used
//#define PIN_SCLK currently not needed, because default pin is used



TCP_Socket_Communication my_tcp_socket(PORT,HOST_IP_ADDR);
AsyncWebServer server(80);
SemaphoreHandle_t xMutex;


//declare functions
void handle_position_data(); 
void handle_haptic_feedback();
void handle_delay(long a);

void handle_position_data_task(void* pvParameters){//task to handle the position data. runns on the first core(0)
  while(1){
    long start_time=0;
    if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE){//function to take the mutex if it is available
      start_time = millis();
      handle_position_data(); //function to handle the position data
      AsyncElegantOTA.loop(); //function to handle the ota update. needs to be called in the loop
      xSemaphoreGive(xMutex); //function to give the mutex back
    }
    vTaskDelay(20-(millis()-start_time)/portTICK_PERIOD_MS); //delay to run the task every 20ms->50Hz
  }
}

void handle_haptic_feedback_task(void* pvParameters){//task to handle the haptic feedback. runns on the second core(1)
  while(1){
    long start_time=0;
    if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE){ //function to take the mutex if it is available
      start_time = millis();
      handle_haptic_feedback(); //function to handle the haptic feedback
      AsyncElegantOTA.loop(); //function to handle the ota update. needs to be called in the loop
      xSemaphoreGive(xMutex); //function to give the mutex back
    }
    vTaskDelay(20-(millis()-start_time)/portTICK_PERIOD_MS);//delay to run the task every 20ms->50Hz
  }
}


void setup() {
  
  Serial.begin(115200); 

  log_init(LOG_LEVEL_INFO); //set log levels for all modules to info
  
  log_info("setup");

  battery_management_set_fast_charging(); //function to set the fast charging mode for the battery management
  
  i2c_multiplexer_init(); //function to initialize the i2c multiplexer
  log_info("i2c multiplexer init done");
  
  bno_imu_init(); //function to initialize the imu

  //log_info("bno init done");

  drv_init(); //function to initialize the drv driver

  log_info("drvinit done");
  
  communication_connect_wifi(WIFI_SSID,WIFI_PASSWORT); //function to connect to the wifi
  log_info("wifi connected");	
  log_info(String(WiFi.localIP().toString())+"");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "OTA_Update for ESP_Haptic_Feedback");
  }); //function to set up the http server for ota updates
  AsyncElegantOTA.begin(&server); //function to set up the ota update
  server.begin(); //function to start the http server for ota updates
  Serial.println("HTTP server started");

  my_tcp_socket.tcp_socket_init(); //function to initialize the tcp socket
  while(my_tcp_socket.tcp_socket_connect()!=0){ //function to connect to the tcp master device
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
  log_info("tcp connected");
  if(ENABLE_MULTITASKING==1){
    xMutex = xSemaphoreCreateMutex(); //function to create a mutex used by the parallel tasks
    xTaskCreatePinnedToCore(handle_position_data_task, "handle_position_data_task", 10000, NULL, 1, NULL, 1); //function to create a task for handling the position data
    xTaskCreatePinnedToCore(handle_haptic_feedback_task, "handle_haptic_feedback_task", 10000, NULL, 1, NULL, 0); //function to create a task for handling the haptic feedback
    delay(1000);
  }
}


void loop() {
  if(ENABLE_MULTITASKING==0){//execute the following code in a super-loop
    long start_time = millis();
    handle_position_data();
    handle_haptic_feedback();
    AsyncElegantOTA.loop(); //function to handle the ota update. needs to be called in the loop
    handle_delay(start_time);
  }
}



/**
 * @brief Function for delaying the loop to get the desired update interval of 20 ms. If the loop takes longer than 20 ms, the delay is skipped. 
 * @param a time when the loop started
 */
void handle_delay(long a){
  log_info(String("time needed for loop: ")+String(millis()-a));
  int delay_time = UPDATEINTERVAL_MS-(millis()-a);
  if (delay_time>0){
    vTaskDelay(delay_time/portTICK_PERIOD_MS);
  }
}


/**
 * @brief Function for dealing with the process for getting data from the imu to the master
 * 
 */
void handle_position_data(){
  //long a=millis();
  char p[TCP_SOCKET_SEND_SIZE];
  bno_imu_get_sensor_data_struct_char(p);//get orientation and acceleration data formatted as json string from the imu
  if(strlen(p)<=1){ //if no data is received or imu couldnt be initialized/found, skip round
    log_error("data couldnt be read for the bn055"); 
    return;
  }
  log_debug(p);
  if(my_tcp_socket.tcp_socket_send_string(p,sizeof(p))==TCP_SOCKET_SEND_SIZE){ //send data to the master, success if the size of the data is equal to the bits send
    //log_info("successful sent and collected data in ms: "+ String(millis()-a));
  }else{
    //could handle error here, else just skip round
  }
  
}

/**
 * @brief Function for dealing with the process for getting data from ther master to the drv driver
 * 
 */
void handle_haptic_feedback(){
  //long a=millis();
  char buff[TCP_SOCKET_READ_SIZE]; //including zero ending char
  strcpy(buff,my_tcp_socket.tcp_socket_receive_string_non_blocking());//non blocking mode -> no message skip round
  if(strlen(buff)<=1){
    //log_trace("no new data");//can skip input msg for debugging possible
    return;
  }else{
    StaticJsonDocument<200> doc; 
    DeserializationError error = deserializeJson(doc,buff);//deserialize json string to json object
    // Empyt Input error when no msg received  ;  Invalid Input error when json String not complete
    if(error){
      log_error(error.f_str());
      return;
    }else{
      uint8_t drv_data_arr[5]; //array for the data for the drv driver
      for(uint8_t i=0;i<DRV_DEVICE_COUNTER;i++){
        drv_data_arr[i]=doc[String(i)];//read the data for each drv device from the json object
        drv_run_realtime(i,drv_data_arr[i]); //call the function to run the drv driver with the given intensity
        log_trace(String(drv_data_arr[i]));
      }
      //log_info("successful received data in ms: "+ String(millis()-a));
    }
  }
}

