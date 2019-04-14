#include "Runner.h"

/**
 * Source file for the Contour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new contour object given the vector of points. 
 */
Contour::Contour(std::vector<cv::Point> points) {
    this->points = points;
}