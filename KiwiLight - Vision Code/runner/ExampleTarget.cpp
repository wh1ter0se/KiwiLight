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
                //remove contour at i and exampleContour at k
                std::vector<Contour> newImageContours = std::vector<Contour>();
                std::vector<ExampleContour> newTargetContours = std::vector<ExampleContour>();
                
                //rebuild imageContours and targetContours without the good contour
                for(int a=0; a<imageContours.size(); a++) {
                    if(a != i) {
                        newImageContours.push_back(imageContours[a]);
                    }
                }

                for(int b=0; b<targetContours.size(); b++) {
                    if(b != k) {
                        newTargetContours.push_back(targetContours[b]);
                    }
                }

                imageContours = newImageContours;
                targetContours = newTargetContours;

                i--;
                k--;
            }
        }
    }

    return (targetContours.size() == 0);
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
    switch(prop) {
        case TargetProperty::DIST_X:
            GetExampleContourByID(contour).SetDistX(values);
            break;
        case TargetProperty::DIST_Y:
            GetExampleContourByID(contour).SetDistY(values);
            break;
        case TargetProperty::ANGLE:
            GetExampleContourByID(contour).SetAngle(values);
            break;
        case TargetProperty::SOLIDITY:
            GetExampleContourByID(contour).SetSolidity(values);
            break;
        case TargetProperty::MINIMUM_AREA:
            GetExampleContourByID(contour).SetMinimumArea((int) values.Value());
            break;
    }
}


SettingPair ExampleTarget::GetContourProperty(int contour, TargetProperty prop) {

    SettingPair finalValue = SettingPair(-1,-1);
    switch(prop) {
        case TargetProperty::DIST_X:
            finalValue = this->contours[contour].DistX();
            break;
        case TargetProperty::DIST_Y:
            finalValue = this->contours[contour].DistY();
            break;
        case TargetProperty::ANGLE:
            finalValue = this->contours[contour].Angle();
            break;
        case TargetProperty::ASPECT_RATIO:
            finalValue = this->contours[contour].AspectRatio();
            break;
        case TargetProperty::SOLIDITY:
            finalValue = this->contours[contour].Solidity();
            break;
        case TargetProperty::MINIMUM_AREA:
            finalValue = SettingPair((double) this->contours[contour].MinimumArea(), 0);
            break;
    }

    return finalValue;
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