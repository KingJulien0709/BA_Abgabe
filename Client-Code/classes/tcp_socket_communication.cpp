#include "tcp_socket_communication.hpp"


/**
 * @brief Construct a new TCP Socket Communication object the tcp communication with the master server
 *  AF_INET - ip4
 *  SOCK_STREAM - TCP
 *  IPPROTO_TCP - protocol
 * 
 * @param port Port for the communication
 * @param ip_address ip4 address of the server-master
 */    
TCP_Socket_Communication::TCP_Socket_Communication(int port, const char* ip_address){
    blocking_mode_=true;
    port_=port;
    ip_address_=ip_address;
    sock_struct_local_.sin_addr.s_addr=inet_addr(ip_address);
    sock_struct_local_.sin_port=htons(port);
    sock_struct_local_.sin_family = AF_INET;   
}

void TCP_Socket_Communication::tcp_socket_init(){
    socket_num_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//create socket with Address Familiy ip4, TCP, protocol TCP
    if(socket_num_<0){ //if socket_num_ is negative, an error occured and socket could not be created
        log_error("can not create socket");
        //TODO: add error handling and decide what to do if socket can not be created
        return; //return without initializing the socket
    }
    log_info("Socket with num " + String(socket_num_));
}

void TCP_Socket_Communication::tcp_socket_reinit(){
    tcp_socket_close(); //close socket to free the port
    tcp_socket_init(); //init socket again
}

/**
 * @brief connect the socket to the master server
 * 
 * @return int Value of the connect function. 0 if no error occured.
 */
int TCP_Socket_Communication::tcp_socket_connect(){
    int error = connect(socket_num_,(struct sockaddr*) &sock_struct_local_, sizeof(sock_struct_local_));
    if(error != 0){
        log_error("Error when connecting socket:" + String(error));
        tcp_socket_reinit(); //reinit socket to free the port
    }
    return error;
}
int TCP_Socket_Communication::tcp_socket_close(){
    return closesocket(socket_num_);
}

void TCP_Socket_Communication::tcp_socket_reconnect(){
    tcp_socket_reinit();
     if(tcp_socket_connect()!=0){
        log_error("Error when connecting socket");
        vTaskDelay(5000/portTICK_PERIOD_MS); //wait 5 seconds before trying to reconnect again
     }
}

/**
 * @brief sends data with a given length to the master.
 * 
 * @param data Char array with data to send
 * @param len Length of the data
 * @return int amount of bytes sent. If an error occured, the value is negative or 
 * 0 if no data was sent.
 */
int TCP_Socket_Communication::tcp_socket_send_string(char *data,uint8_t len){
    if(!blocking_mode_){
        tcp_socket_configure_block_mode(true);// send function has to be in blocking mode and wait until data is sent, because the data can be important for the master. 
    }
    int sent_bytes_amount = send(socket_num_,data,len,0); //send data to the master. the return value is the amount of bytes sent.
    if(sent_bytes_amount<=0){ //insufficient bytes sent or error occured
        log_error("Error when sending data:" + String(sent_bytes_amount));
        tcp_socket_reconnect();
    }else{
        log_debug("Bytes sent:" + String(sent_bytes_amount));
    }   
    return sent_bytes_amount;
}

/**
 * @brief receive data from master with blocking mode, which means the function waits until data is received
 * 
 * @return char* array with received data with length of TCP_SOCKET_READ_SIZE. It will be empty if no data is received or an error occured.
 */
char* TCP_Socket_Communication::tcp_socket_receive_string_blocking(){
    if(!blocking_mode_){
        tcp_socket_configure_block_mode(true);//make sure the socket is in the right mode
    }
    static char data[TCP_SOCKET_READ_SIZE];
    int length = recv(socket_num_,data,sizeof(data)-1,0); //blocking mode -> wait until data is received. The data wll be stored in the data char array.
    if(length < 0){
       log_error("Error " + String(length));
       data[0]= 0; 
    }else if(length==0){
        //connection closed by the master -> try to reconnect
        tcp_socket_reconnect(); 
        data[0]= 0;
    }else{
        data[length] = 0;
        log_debug("[tcp_socket_communication]: received data:" + String(data));
    }
    return data;
}
/**
 * @brief Receive function with non_blocking mode and MSG_DONTWAIT 
 * If blocking mode is disabled and the MSG_DONTWAIT flag is set at the receive function, the function does not wait for the socket receive function to finish.
 * An error will be return by the socket function and an empty arr will be returned by this function
 * 
 * @return char* 
 */
char* TCP_Socket_Communication::tcp_socket_receive_string_non_blocking(){
    if(blocking_mode_){
        tcp_socket_configure_block_mode(false);//make sure the socket is in the right mode
    }
    static char data[TCP_SOCKET_READ_SIZE];
    int length = recv(socket_num_,data,sizeof(data)-1,MSG_DONTWAIT);//non blocking mode -> no message skip round. else the return value is the bits received. The data wll be stored in the data char array.
    if(length < 0){
       log_trace("Not enougth bytes received:" + String(length));// trace message, because errors will happen a lot and can be more common than successful runs. But should not effect the program.
       data[0]= 0; 
    }else if(length==0){
        //connection closed by the master -> try to reconnect
        tcp_socket_reconnect();
        data[0]= 0;
    }else{
        data[length] = 0;
        log_debug("received data:" + String(data));
    }
    return data;
}


/**
 * @brief configure the socket to a blocking or non_blocking socket.
 * If blocking mode is enabled, the function waits for the socket operation(mostly receive function) to finish.
 * If blocking mode is disabled and the MSG_DONTWAIT flag is set at the receive function, the function does not wait for the socket receive function to finish.
 * 
 * 
 * @param block_mode BOOLEAN if the program should set to blocking mode.
 */
void TCP_Socket_Communication::tcp_socket_configure_block_mode(bool block_mode){
    if(blocking_mode_==block_mode){
        return;//no need for changing mode
    }else{
        //more efficient solution if only the 0_NONBLOCK flag is used
        if(!block_mode){
            fcntl(socket_num_,F_SETFL,O_NONBLOCK);//set to the non_blocking mode. Socket operation will not wait until finished, but can return an error if not finished properly.
        }else{
            fcntl(socket_num_,F_SETFL,0);//set to the blocking mode. Socket operation will wait until finished.
        }
        //general solution if other flags are set. In this class there is just the 0_NONBLOCK flag. So the simple implmentation is enougth. if other flags are used, this solution should be used.
        //int flags = fcntl(socket_num_,F_GETFL,0);
        //if(block_mode){
        //    fcntl(socket_num_,F_SETFL,flags | O_NONBLOCK);// c
        //}else{
        //    fcntl(socket_num_,F_SETFL,flags & ~O_NONBLOCK);
        //}
    }
}