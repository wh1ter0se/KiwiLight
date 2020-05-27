#include "Runner.h"

/**
 * Source file for the CameraFrame class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new CameraFrame describing information from the passed Mat.
 * Precondition: "img" is already preprocessed.
 */
CameraFrame::CameraFrame(Mat img, int minimumArea) {
    img.copyTo(this->image);
    
    //find the contours
    std::vector <std::vector <Point> > contours;
    findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //wrap the contours
    std::vector<Contour> targetContours;
    for(int i=0; i<contours.size(); i++) {
        Contour newContour = Contour(contours[i]);

        if(newContour.Area() > minimumArea) {
            targetContours.push_back(newContour);
        }
    }

    this->seenTarget = Target(0, targetContours, 0.0, 0.0, 0.0, 0.0, DistanceCalcMode::BY_WIDTH);
}

/**
 * Groups the contours by their position in the image.
 */
std::vector<Contour> CameraFrame::GetContoursGrouped() {
    std::vector<Contour> contours = this->seenTarget.Contours();
    std::vector<Contour> groupedContours;

    for(int i=0; i<contours.size(); i++) {
        //get the contour to insert
        Contour contourToInsert = contours[i];
        int insertPosValue = PositionValue(contourToInsert);

        //if the contour has a smaller horizontal distance than the first in the group, or if there is nothing in the group so far
        if(groupedContours.size() == 0 || PositionValue(groupedContours[0]) > insertPosValue) {
            groupedContours.push_back(contourToInsert);
        } else {
            //find a place in the vector to insert the contour
            bool objectInserted = false;
            for(int k=1; k<groupedContours.size(); k++) {
                Contour contourBeforeK = groupedContours[k - 1];
                Contour contourAtK = groupedContours[k];
            
                if(PositionValue(contourBeforeK) < insertPosValue &&
                   PositionValue(contourAtK) >= insertPosValue) {
                    
                    //insert HERE
                    groupedContours.emplace(groupedContours.begin() + k);
                    objectInserted = true;
                }
            }
            
            //if the contour does not belong anywhere else...
            if(!objectInserted) {
                groupedContours.emplace(groupedContours.end(), contourToInsert);
            }
        }
    }

    return groupedContours;
}

/**
 * Calculates a unique value describing the pixel the contour's center is positioned at. Values start at 0 for the topmost, leftmost pixel,
 * and increase by 1 for each pixel following it to the right and down, such as one would read a book.
 * Some examples, assuming that this CameraFrame has a width of 240 and a height of 120:
 * - This method will return 0 for a contour who's center is at (0, 0)
 * - This method will return 7 for a contour who's center is at (7, 0)
 * - This method will return 244 for a contour who's center is at (2, 1);
 * - This method will return 32,948 for a contour who's center is at (68, 137)
 */
int CameraFrame::PositionValue(Contour contour) {
    int imgWidth = this->image.cols;
    
    int pointValue = (contour.Center().y * imgWidth) + contour.Center().x;
    return pointValue;
}

/**
 * Calculates and returns the distance from "contour" to the center of the target, in target widths.
 */
Distance CameraFrame::GetContourDistance(Contour contour) {
    Point targetCenter = this->seenTarget.Center();

    //find the amount of contour widths to the center of the target and return the distance

    double targWidth = this->seenTarget.Bounds().width;
    double xDifference = targetCenter.x - contour.Center().x;
    double yDifference = targetCenter.y - contour.Center().y;

    xDifference /= targWidth;
    yDifference /= targWidth;

    return Distance(xDifference, yDifference);
}
