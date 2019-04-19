#include "Runner.h"

/**
 * Source file for the ExampleTarget class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new ExampleTarget with given ExampleContours, to model a real target.
 */
ExampleTarget::ExampleTarget(int id, std::vector<ExampleContour> contours) {
    this->id = id;
    this->contours = contours;
}

/**
 * returns a vector containing all targets found within the vector of contours.
 */
std::vector<Target> ExampleTarget::GetTargets(std::vector<Contour> contours) {

}

/**
 * Given the vector of contours, returns whether the contours could be a target(true) or not(false).
 */
bool ExampleTarget::isTarget(std::vector<Contour> contours) {
    
}