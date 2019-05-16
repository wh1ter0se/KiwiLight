#include "Util.h"

/**
 * source file for UDP class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

void err(const char *msg) {
    perror(msg);
    exit(1);
}

/**
 * Construct a new UDP object connecting to dest_ip on port
 * @param dest_ip a string containing the ip of the server in standard dot notation
 * @param port the port to connect on
 */
UDP::UDP(std::string dest_ip, int port) {

    std::cout << "Setting up UDP..." << std::endl;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        err("SOCKET FAILED");

    memset(&this->server_address, 0, sizeof(this->server_address));
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(port);

    //do inet_pton()
    int pton_result = inet_pton(AF_INET, dest_ip.c_str(), &this->server_address.sin_addr);
    if(pton_result <= 0) 
        err("PTON FAILED");

    //wait for the connect to succeed before we go
    std::cout << "Waiting to connect..." << std::endl;
    while(true) {
        //now connect
        int connect_result = connect(this->sock, (sockaddr*) &this->server_address, sizeof(this->server_address));
        if(connect_result > -1) //the connection was the big good
            break;
    }

    std::cout << "Connection set up." << std::endl;
}

/**
 * Sends the given message to the destination
 * @param msg a string containing the message to send
 */
void UDP::Send(std::string msg) {
    const char *buffer = msg.c_str();
    int send_result = send(this->sock, buffer, strlen(buffer), 0); //the big send
    // if(send_result < 0)
    //     std::cout << "SEND FAILED!";
    //     std::cout.flush();
}

/**
 * Reads the buffer and returns whats there
 * @return a string containing the contents of the buffer
 */
std::string UDP::Recieve() {
    char buffer[1024] = {0};
    int read_result = read(this->sock, buffer, 1023);
    return buffer;
}

/**
 * closes the UDP socket
 */
void UDP::Close() {
    close(this->sock);
}
