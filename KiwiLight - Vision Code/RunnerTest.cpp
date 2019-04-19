#include "KiwiLight.h"

/**
 * Test program for Runner
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


int main() {
    std::cout << "hello world!" << std::endl;
    Runner run = Runner("confs/test_conf.xml", true);

    run.Loop();
}