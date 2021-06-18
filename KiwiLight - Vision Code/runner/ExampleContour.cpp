#include "Runner.h"

/**
 * Source file for the ExampleContour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates an instance of the default ExampleContour, a square.
 */
ExampleContour::ExampleContour(int id) {
    this->id = id;
    this->distX = SettingPair(0, 0.25);
    this->distY = SettingPair(0, 0.25);
    this->angle = SettingPair(0, 90);
    this->aspectRatio = SettingPair(1, 0.25);
    this->solidity = SettingPair(1, 0.125);
    this->minimumArea = 500;
}

/**
 * Creates an ExampleContour to model a real contour using the given information.
 */
ExampleContour::ExampleContour(int id, SettingPair distX, SettingPair distY, SettingPair angle,SettingPair aspectRatio,SettingPair solidity,int minimumArea) {
    this->id = id;
    this->distX = distX;
    this->distY = distY;
    this->angle = angle;
    this->aspectRatio = aspectRatio;
    this->solidity = solidity;
    this->minimumArea = minimumArea;
}

/**
 * Analyzes the passed contour object and returns whether or not it fits the criteria of this ExampleContour.
 * @param contour The contour to test.
 * @return True if the contour represents this ExampleContour, false otherwise.
 */
bool ExampleContour::IsContour(Contour contour) {
    bool angleTest = false;
    bool arTest = false;
    bool solidTest = false;

    //do the area test first to optimize
    bool areaTest = (contour.Area() > this->minimumArea);
    if(!areaTest) {
        return false;
    }

    //conduct all five tests and return result of all five

    bool angleLower = (contour.Angle() > this->angle.LowerBound());
    bool angleUpper = (contour.Angle() < this->angle.UpperBound());
    angleTest = angleLower && angleUpper;

    bool arLower = (contour.AspectRatio() > this->aspectRatio.LowerBound());
    bool arUpper = (contour.AspectRatio() < this->aspectRatio.UpperBound());
    arTest = arLower && arUpper;

    bool solidLower = (contour.Solidity() > this->solidity.LowerBound());
    bool solidUpper = (contour.Solidity() < this->solidity.UpperBound());
    solidTest = solidLower && solidUpper;


    return (angleTest && arTest && solidTest && areaTest);
}

/**
 * Sets the value and allowable error of this ExampleContour's DistX property (target widths from contour center to target center, horizontally).
 */
void ExampleContour::SetDistX(SettingPair distX) {
    this->distX = distX;
}

/**
 * Sets the value and allowable error of this ExampleContour's DistY property (target heights from contour center to target center, vertically).
 */
void ExampleContour::SetDistY(SettingPair distY) {
    this->distY = distY;
}

/**
 * Sets the value and allowable error of this ExampleContour's Angle property (degrees from positive X axis).
 */
void ExampleContour::SetAngle(SettingPair angle) {
    this->angle = angle;
}

/**
 * Sets the value and allowable error of this ExampleContour's Aspect Ratio property (width / height).
 */
void ExampleContour::SetAspectRatio(SettingPair aspectRatio) {
    this->aspectRatio = aspectRatio;
}

/**
 * Sets the value and allowable error of this ExampleContour's Solidity property (shape area / bounding box area)
 */
void ExampleContour::SetSolidity(SettingPair solidity) {
    this->solidity = solidity;
}

/**
 * Sets the value of this ExampleContour's Minimum Area property (area required for any contour to not be ignored).
 */
void ExampleContour::SetMinimumArea(int minimumArea) {
    this->minimumArea = minimumArea;
}
