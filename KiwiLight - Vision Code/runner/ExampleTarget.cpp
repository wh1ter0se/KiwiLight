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
ExampleTarget::ExampleTarget(int id, std::vector<ExampleContour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance) {
    this->id = id;
    this->contours = contours;
    this->knownHeight = knownHeight;
    this->focalHeight = focalHeight;
    this->distErrorCorrect = distErrorCorrect;
    this->calibratedDistance = calibratedDistance;
}

/**
 * returns a vector containing all targets found within the vector of contours.
 */
std::vector<Target> ExampleTarget::GetTargets(std::vector<Contour> objects) {
    std::vector<Target> foundTargets = std::vector<Target>();
    std::vector<Contour> validContours = std::vector<Contour>();

    //find valid contours
    for(int i=0; i<objects.size(); i++) {
        for(int k=0; k<this->contours.size(); k++) {
            if(this->contours[k].IsContour(objects[i])) {
                validContours.push_back(objects[i]);
                break;
            }
        }
    }

    int numTargetContours = this->contours.size();
    int numImageContours = validContours.size();

    if(numImageContours >= numTargetContours) {

        if(numTargetContours == 1) {
            for(int i=0; i<numImageContours; i++) {
                std::vector<Contour> potentialTarget = std::vector<Contour>();
                potentialTarget.push_back(validContours[i]);
                if(isTarget(potentialTarget)) {
                    Target newTarg = Target(this->id, potentialTarget, this->knownHeight, this->focalHeight, this->distErrorCorrect, this->calibratedDistance);
                    foundTargets.push_back(newTarg);
                }
            }
        } 
        else {
            int places[(const int) numTargetContours] = {};
            for(int i=0; i<numTargetContours; i++) {
                places[i] = 0;
            }

            //while new combos are possible, generate and test combos of targets
            while(!ArrayMaxed(places, numTargetContours, numImageContours)) {
                std::string out = "";
                if(!ContainsDuplicates(places, numTargetContours)) {
                    std::vector<Contour> potentialTarget = std::vector<Contour>();
                    for(int i=0; i<numTargetContours; i++) {
                        int index = places[i];
                        potentialTarget.push_back(objects[index]);
                        out += std::to_string(index) + " ";
                    }

                    //test new target
                    if(isTarget(potentialTarget)) {
                        Target newTarget = Target(this->id, potentialTarget, this->knownHeight, this->focalHeight, this->distErrorCorrect, this->calibratedDistance);
                        foundTargets.push_back(newTarget);
                    }
                }

                for(int i=0; i<numTargetContours; i++) {
                    places[i]++;

                    if(i > 0) {
                        places[i-1] = 0;
                    }

                    if(places[i] < numImageContours) {
                        break;
                    }   
                }
            }
        }
    }

    return foundTargets;
}

/**
 * Given the vector of contours, returns whether the contours could be a target(true) or not(false).
 * precondition: objects.size() == this->contours.size();
 */
bool ExampleTarget::isTarget(std::vector<Contour> objects) {
    /**
     * Determine if the passed vector of contours is a target or not by comparing the 
     * distances from the center of the target in target widths.
     * if a contour passes, it is removed from the array.
     * if the array is empty by the end of the method, return true.
     */

    std::vector<Contour> imageContours = objects;
    std::vector<ExampleContour> targetContours = this->contours;

    int totalGood = 0;

    //find center of target by averaging the x and y coordinates
    int centerX = 0;
    int centerY = 0;
    for(int i=0; i<objects.size(); i++) {
        centerX += objects[i].X();
        centerY += objects[i].Y();
    }

    centerX /= objects.size();
    centerY /= objects.size();

    for(int i=0; i<imageContours.size(); i++) {
        Contour object = imageContours[i];
        //determine how many widths to the center for the object and compare to our targets
        int width = object.Width();
        for(int k=0; k<targetContours.size(); k++) {
            //measure distance in pixels, convert to widths, and compare to exampleContours.
            int distToCenterX = centerX - object.X();
            double widthsToCenterX = distToCenterX / (double) width;

            bool distXValid = (widthsToCenterX > targetContours[k].DistX().LowerBound() &&
                               widthsToCenterX < targetContours[k].DistX().UpperBound() );
            
            int distToCenterY = centerY - object.Y();
            double widthsToCenterY = distToCenterY / (double) width;

            bool distYValid = (widthsToCenterY > targetContours[k].DistY().LowerBound() &&
                               widthsToCenterY < targetContours[k].DistY().UpperBound() );

            if(distXValid && distYValid && targetContours[k].IsContour(imageContours[i])) {
                totalGood++;
            }
        }
    }

    return (this->contours.size() == totalGood);
}

/**
 * Returns the exampleContour with "id". returns the first contour if none found.
 */
ExampleContour ExampleTarget::GetExampleContourByID(int id) {
    for(int i=0; i<this->contours.size(); i++) {
        if(this->contours[i].ID() == id) {
            return this->contours[i];
        }
    }
    std::cout << "WARNING: ExampleContour with id " << id << " does not exist in target with id " << this->ID() << "!" << std::endl;
    return this->contours[0];
}


void ExampleTarget::SetContourProperty(int contour, TargetProperty prop, SettingPair values) {  
    //get the index of the contour we want to change, that way we directly set the values instead of taking a reference
    int contourIndex = 0;
    for(int i=0; i<this->contours.size(); i++) {
        if(this->contours[i].ID() == contour) {
            contourIndex = i;
            break;
        }
    }

    switch(prop) {
        case TargetProperty::DIST_X:
            this->contours[contourIndex].SetDistX(values);
            break;
        case TargetProperty::DIST_Y:
            this->contours[contourIndex].SetDistY(values);
            break;
        case TargetProperty::ANGLE:
            this->contours[contourIndex].SetAngle(values);
            break;
        case TargetProperty::ASPECT_RATIO:
            this->contours[contourIndex].SetAspectRatio(values);
            break;
        case TargetProperty::SOLIDITY:
            this->contours[contourIndex].SetSolidity(values);
            break;
        case TargetProperty::MINIMUM_AREA:
            this->contours[contourIndex].SetMinimumArea((int) values.Value());
            break;
    }
}


SettingPair ExampleTarget::GetContourProperty(int contour, TargetProperty prop) {
    SettingPair finalValue = SettingPair(-1,-1);

    int contourIndex = 0;
    for(int i=0; i<this->contours.size(); i++) {
        if(this->contours[i].ID() == contour) {
            contourIndex = i;
            break;
        }
    }

    switch(prop) {
        case TargetProperty::DIST_X:
            finalValue = this->contours[contourIndex].DistX();
            break;
        case TargetProperty::DIST_Y:
            finalValue = this->contours[contourIndex].DistY();
            break;
        case TargetProperty::ANGLE:
            finalValue = this->contours[contourIndex].Angle();
            break;
        case TargetProperty::ASPECT_RATIO:
            finalValue = this->contours[contourIndex].AspectRatio();
            break;
        case TargetProperty::SOLIDITY:
            finalValue = this->contours[contourIndex].Solidity();
            break;
        case TargetProperty::MINIMUM_AREA:
            finalValue = SettingPair((double) this->contours[contourIndex].MinimumArea(), 0);
            break;
    }

    return finalValue;
}


void ExampleTarget::SetTargetProperty(RunnerProperty prop, double value) {
    switch(prop) {
        case RunnerProperty::TRUE_WIDTH:
            this->knownHeight = value;
            break;
        case RunnerProperty::PERCEIVED_WIDTH:
            this->focalHeight = value;
            break;
        case RunnerProperty::CALIBRATED_DISTANCE:
            this->calibratedDistance = value;
            break;
        case RunnerProperty::ERROR_CORRECTION:
            this->distErrorCorrect = value;
            break;
    }
}


double ExampleTarget::GetTargetProperty(RunnerProperty prop) {
    double value = -1;

    switch(prop) {
        case RunnerProperty::TRUE_WIDTH:
            value = this->knownHeight;
            break;
        case RunnerProperty::PERCEIVED_WIDTH:
            value = this->focalHeight;
            break;
        case RunnerProperty::CALIBRATED_DISTANCE:
            value = this->calibratedDistance;
            break;
        case RunnerProperty::ERROR_CORRECTION:
            value = this->distErrorCorrect;
            break;
    }

    return value;
}


bool ExampleTarget::ArrayMaxed(int arr[], int size, int max) {
    if(arr[size - 1] < max) {
        return false;
    }

    return true;
}


bool ExampleTarget::ContainsDuplicates(int arr[], int size) {
    for(int i=0; i<size; i++) {
        for(int k=0; k<size; k++) {
            if(i != k && arr[i] == arr[k]) {
                //thats a duplicate
                return true;
            }
        }
    }
}