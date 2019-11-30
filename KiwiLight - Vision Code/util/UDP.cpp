#include "Util.h"

/**
 * source file for UDP class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


/**
 * literally kills everything
 */
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
    this->address = dest_ip;
    this->port = port;

    std::cout << "Setting up UDP..." << std::endl;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        err("SOCKET FAILED");

    memset(&this->client_address, 0, sizeof(this->client_address));
    this->client_address.sin_family = AF_INET;
    this->client_address.sin_port = htons(port);

    //do inet_pton()
    int pton_result = inet_pton(AF_INET, dest_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result <= 0) 
        err("PTON FAILED");

    //wait for the connect to succeed before we go
    std::cout << "Waiting to connect..." << std::endl;
    while(true) {
        //now connect
        int connect_result = connect(this->sock, (sockaddr*) &this->client_address, sizeof(this->client_address));
        if(connect_result > -1) //the connection was the big good
            break;
    }

    std::cout << "Connection set up." << std::endl;
}


UDP::UDP(std::string dest_ip, int port, bool blockUntilConnected) {
    this->address = dest_ip;
    this->port = port;

    std::cout << "Setting up UDP..." << std::endl;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        err("SOCKET FAILED");

    memset(&this->client_address, 0, sizeof(this->client_address));
    this->client_address.sin_family = AF_INET;
    this->client_address.sin_port = htons(port);

    //do inet_pton()
    int pton_result = inet_pton(AF_INET, dest_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result <= 0) 
        err("PTON FAILED");

    //wait for the connect to succeed before we go
    std::cout << "Waiting to connect..." << std::endl;
    while(true) {
        //now connect
        int connect_result = connect(this->sock, (sockaddr*) &this->client_address, sizeof(this->client_address));
        if(connect_result > -1 || !blockUntilConnected) //the connection was the big good
            break;
    }

    std::cout << "Connection set up." << std::endl;
}


UDP::UDP(std::string this_ip, std::string dest_ip, int port, bool blockUntilConnected) {
    this->address = dest_ip;
    this->port = port;

    std::cout << "Setting up UDP..." << std::endl;
    

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        err("SOCKET FAILED");

    //define destination address
    memset(&this->client_address, 0, sizeof(this->client_address));
    this->client_address.sin_family = AF_INET;
    this->client_address.sin_port = htons(port);
    
    //define our address
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    //convert "this_ip" to a usable address struct
    int pton_result_1 = inet_pton(AF_INET, this_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result_1 <= 0)
        err("THIS PTON FAILED");

    //convert "dest_ip" to a usable address struct
    int pton_result_2 = inet_pton(AF_INET, dest_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result_2 <= 0) 
        err("DEST PTON FAILED");
        
    
    //"bind" our address to the socket
    int bind_result = bind(this->sock, (sockaddr*) &server_address, sizeof(server_address));
    if(bind_result < 0)
        err("BIND FAILED");

    //wait for the connect to succeed before we go
    std::cout << "Waiting to connect..." << std::endl;
    while(true) {
        //now connect
        int connect_result = connect(this->sock, (sockaddr*) &this->client_address, sizeof(this->client_address));
        if(connect_result > -1 || !blockUntilConnected) //the connection was the big good
            break;
    }

    std::cout << "Connection set up." << std::endl;
}

/**
 * Attempts to connect to the router.
 * @return true if the connection was successful, or the UDP was already connected, false otherwise.
 */
bool UDP::AttemptToConnect() {
    if(!this->connected) {
        int connect_result = connect(this->sock, (sockaddr*) &this->client_address, sizeof(this->client_address));
        
        if(connect_result < 0) {
            std::cout << "CONNECT FAILED" << std::endl;
        }
        return (connect_result > -1);
    }

    return true;
}

/**
 * Sends the given message to the destination
 * @param msg a string containing the message to send
 */
void UDP::Send(std::string msg) {
    const char *buffer = msg.c_str();
    //std::cout << msg << std::endl;
    int send_result = send(this->sock, buffer, strlen(buffer), 0); //the big send
}

/**
 * Reads the buffer and returns whats there
 * @return a string containing the contents of the buffer
 */
std::string UDP::Recieve() {
    char buffer[1024] = {0};
    int read_result = recv(this->sock, buffer, 1024, MSG_DONTWAIT);
    std::string bufferStr = std::string(buffer);
    return bufferStr;
}

/**
 * closes the UDP socket
 */
void UDP::Close() {
    close(this->sock);
}
