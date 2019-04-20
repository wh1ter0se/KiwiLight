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
ExampleContour::ExampleContour(int id, Distance distFromCenter, int width, int height, int angle, double solidity) {
    this->id = id;
    this->distFromCenter = distFromCenter;
    this->width = width;
    this->height = height;
    this->angle = angle;
    this->solidity = solidity;
}


bool ExampleContour::IsContour(Contour contour) {
    return true;
}