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

    this->seenTarget = Target(0, targetContours, 0.0, 0.0, 0.0, 0.0);
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


int CameraFrame::PositionValue(Contour contour) {
    int imgWidth = this->image.cols;
    
    int pointValue = (contour.Center().y * imgWidth) + contour.Center().x;
    return pointValue;
}


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
