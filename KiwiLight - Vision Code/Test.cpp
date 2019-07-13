#include "KiwiLight.h"

/**
 * TEST FILE
 */

using namespace cv;
using namespace KiwiLight;

void EditSelected() {}
void ToggleUDP() {}

int main(int argc, char *argv[]) {
    UDP udp = UDP("10.36.95.1", "10.36.95.16", 3695, true);

    std::string input;

    std::cout << "send: ";
    std::cin >> input;
    
    udp.Send(input);

    std::cout << "sent " << input << std::endl;

    return 0;
}