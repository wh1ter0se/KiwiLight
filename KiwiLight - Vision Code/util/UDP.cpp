#include "Util.h"

/**
 * source file for UDP class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


/**
 * literally kills everything
 * DEPRECATED: This method will be removed in the next update.
 */
[[deprecated("This method will be removed in the next update.")]]
void err(const char *msg) {
    perror(msg);
    exit(1);
}

void reportErr(const char *msg) {
    std::cerr << "ERROR: " << msg << std::endl;
}

/**
 * Creates a new UDP sender.
 * @param dest_ip The desination IPv4 address.
 * @param port The port
 * @param blockUntilConnected True if this call should block until connected, false otherwise.
 */
UDP::UDP(std::string dest_ip, int port, bool blockUntilConnected) {
    this->address = dest_ip;
    this->port = port;
    this->connected = false;

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        reportErr("SOCKET FAILED");

    memset(&this->client_address, 0, sizeof(this->client_address));
    this->client_address.sin_family = AF_INET;
    this->client_address.sin_port = htons(port);

    //do inet_pton(), translating information from our stringed ip address into the address object
    int pton_result = inet_pton(AF_INET, dest_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result <= 0) 
        reportErr("PTON FAILED");

    //wait for the connect to succeed before we go
    while(true) {
        if(AttemptToConnect() || !blockUntilConnected) {
            break;
        }
    }
}

/**
 * Creates a new UDP Sender.
 * @param this_ip The address to bind to.
 * @param dest_ip The destination IPv4 address.
 * @param port The port
 * @param blockUntilConnected True if this call should block until connected, false otherwise.
 * DEPRECATED: This constructor is not used in KiwiLight and will be removed in the next update.
 */
UDP::UDP(std::string this_ip, std::string dest_ip, int port, bool blockUntilConnected) {
    this->address = dest_ip;
    this->port = port; 
    this->connected = false;   

    this->sock = socket(AF_INET, SOCK_DGRAM, 0); //"0" for wildcard of what protocol is best
    if(this->sock < 0) //socket creation didnt be like that tho
        reportErr("SOCKET FAILED");

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
        reportErr("THIS PTON FAILED");

    //convert "dest_ip" to a usable address struct
    int pton_result_2 = inet_pton(AF_INET, dest_ip.c_str(), &this->client_address.sin_addr);
    if(pton_result_2 <= 0) 
        reportErr("DEST PTON FAILED");
        
    
    //"bind" our address to the socket
    int bind_result = bind(this->sock, (sockaddr*) &server_address, sizeof(server_address));
    if(bind_result < 0)
        reportErr("BIND FAILED");

    //wait for the connect to succeed before we go
    std::cout << "Waiting to connect..." << std::endl;
    while(true) {
        if(AttemptToConnect() || !blockUntilConnected) {
            break;
        }
    }

    std::cout << "Connection set up." << std::endl;
}

/**
 * Attempts to connect to the router.
 * @return true if the connection was successful, or the UDP was already connected, false otherwise.
 */
bool UDP::AttemptToConnect() {
    int connect_result = connect(this->sock, (sockaddr*) &this->client_address, sizeof(this->client_address));
    
    if(connect_result < 0) {
        std::cout << "CONNECT FAILED" << std::endl;
        return false;
    }
    
    this->connected = true;
    return true;
}

/**
 * Sends the given message to the destination
 * @param msg a string containing the message to send
 */
void UDP::Send(std::string msg) {
    const char *buffer = msg.c_str();
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
 * Closes the UDP socket
 */
void UDP::Close() {
    close(this->sock);
}
