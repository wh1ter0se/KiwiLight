#include "Util.h"

/**
 * Source file for the Distance class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


Distance::Distance(int x, int y, int error) {
    this->x = x;
    this->y = y;
    this->error = error;
}


int Distance::TrueDistance() {
    double a = pow(x, 2);
    double b = pow(y, 2);
    double c = a + b;
    c = sqrt(c);
    return (int)c;
}