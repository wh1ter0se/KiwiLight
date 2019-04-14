#include "Runner.h"

/**
 * Source file for the ExampleContour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates an ExampleContour to model a real contour using the given information.
 */
ExampleContour::ExampleContour(int x, int y, int width, int height, int angle, int solidity) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->angle = angle;
    this->solidity = solidity;
}