#include "Util.h"

/**
 * source file for UDP class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

void reportErr(const char *msg) {
    std::cerr << "ERROR: " << msg << std::endl;
}

/**
 * Creates a new UDP sender.
 * @param dest_ip The desination IPv4 address.
 * @param port The port
 * @param blockUntilConnected True if this call should block until connected, false otherwise.
 */
UDP::UDP(std::string dest_ip, int port, bool blockUntilConnected, int maxSendRate) {
    this->address = dest_ip;
    this->port = port;
    this->connected = false;
    this->maxSendRate = maxSendRate;
    this->lastSendTime = -1;
    this->clock = Clock();
    this->clock.Start();

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

bool UDP::Connected() {
    return this->connected;
}
/**
 * Sends the given message to the destination
 * @param msg a string containing the message to send
 */
void UDP::Send(std::string msg) {
    //use timer to see if send should be allowed to happen
    long currentTime = clock.GetTime();
    int timeSinceLastSend = currentTime - lastSendTime;
    if(timeSinceLastSend > (1000 / maxSendRate)) {
        const char *buffer = msg.c_str();
        int send_result = send(this->sock, buffer, strlen(buffer), 0); //the big send
        lastSendTime = currentTime;
    }
}

/**
 * Reads the buffer and returns whats there
 * @return a string containing the contents of the buffer
 */
std::string UDP::Receive() {
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

/**
 * Returns the IPv4 address that the UDP sends to.
 */
std::string UDP::GetAddress() {
    return this->address;
}

/**
 * Returns the port that the UDP operates on.
 */
int UDP::GetPort() {
    return this->port;
}

/**
 * Returns the maximum send rate in sends per second.
 */
int UDP::MaxSendRate() {
    return this->maxSendRate;
}

/**
 * Sets the maximum send rate in sends per second.
 */
void UDP::SetMaxSendRate(int maxsendrate) {
    this->maxSendRate = maxsendrate;
}
