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
ExampleTarget::ExampleTarget(
    int id, 
    std::vector<ExampleContour> contours, 
    double knownHeight, 
    double focalHeight, 
    double distErrorCorrect, 
    double calibratedDistance,
    DistanceCalcMode distMode,
    int maxContours
) {
    this->id = id;
    this->contours = contours;
    this->knownHeight = knownHeight;
    this->focalHeight = focalHeight;
    this->distErrorCorrect = distErrorCorrect;
    this->calibratedDistance = calibratedDistance;
    this->distMode = distMode;
    this->maxContours = maxContours;
}

/**
 * returns a vector containing all targets found within the vector of contours.
 */
std::vector<Target> ExampleTarget::GetTargets(std::vector<Contour> objects) {

    std::vector<Target> foundTargets = std::vector<Target>();
    std::vector<Contour> validContours = std::vector<Contour>();
    
    validContours = this->GetValidContours(objects);

    int numTargetContours = this->contours.size();
    int numImageContours = validContours.size();

    if(numTargetContours == 1) { //this is simply for optimization, since we can do 1-contours faster very easily, and FRC likes 1-contours
        for(int i=0; i<validContours.size(); i++) {
            std::vector<Contour> potentialTarget = std::vector<Contour>();
            potentialTarget.push_back(validContours[i]);
                Target newTarg = Target(this->id, potentialTarget, this->knownHeight, this->focalHeight, this->distErrorCorrect, this->calibratedDistance, this->distMode);
                foundTargets.push_back(newTarg);
        }
    } 
    else {
        int places[(const int) numTargetContours] = {};
        for(int i=0; i<numTargetContours; i++) {
            places[i] = 0;
        }

        std::vector< std::vector<int> > originalCombos;

        //while new combos are possible, generate and test combos of contours
        while(!ArrayMaxed(places, numTargetContours, numImageContours)) {
            if(!ContainsDuplicates(places, numTargetContours)) {
                if(!CombonationAlreadyTested(places, originalCombos, numTargetContours)) {
                    //add to original combos and then test

                    //Log this combo so that it does not get duplicated!
                    std::vector<int> newOriginalCombo;
                    for(int i=0; i<numTargetContours; i++) {
                        newOriginalCombo.push_back(places[i]);
                    }
                    originalCombos.push_back(newOriginalCombo);

                    //the testing part
                    std::vector<Contour> potentialTarget = std::vector<Contour>();
                    for(int i=0; i<numTargetContours; i++) {
                        int index = places[i];
                        potentialTarget.push_back(validContours[index]);
                    }

                    //test new target
                    if(isTarget(potentialTarget)) {
                        Target newTarget = Target(this->id, potentialTarget, this->knownHeight, this->focalHeight, this->distErrorCorrect, this->calibratedDistance, this->distMode);
                        foundTargets.push_back(newTarget);
                    }
                }
            }

            //calculate the next combo of contours
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
     * if the array is empty by the end of the method, return true
     */

    std::vector<Contour> imageContours = objects;
    std::vector<ExampleContour> targetContours = this->contours;

    int totalGood = 0;

    Target potentialTarg = Target(0, imageContours, 0, 0, 0, 0, this->distMode);
    int centerX = potentialTarg.Center().x;
    int centerY = potentialTarg.Center().y;
    
    int objectWidth = potentialTarg.Bounds().width;

    for(int i=0; i<imageContours.size(); i++) {
        Contour object = imageContours[i];
        //determine how many widths to the center for the object and compare to our targets
        int distToCenterX = centerX - object.Center().x;
        double widthsToCenterX = distToCenterX / (double) objectWidth;

        int distToCenterY = centerY - object.Center().y;
        double widthsToCenterY = (double) distToCenterY / (double) objectWidth;
        for(int k=0; k<targetContours.size(); k++) {
            //measure distance in pixels, convert to widths, and compare to exampleContours.
            bool distXValid = (widthsToCenterX > targetContours[k].DistX().LowerBound() &&
                               widthsToCenterX < targetContours[k].DistX().UpperBound() );

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
 * Filters out contours that are definitely NOT part of this target.
 */
std::vector<Contour> ExampleTarget::GetValidContours(std::vector<Contour> objects) {
    std::vector<Contour> validContours;
    
    //go through and test contours one-by-one
    for(int i=0; i<objects.size(); i++) {
        for(int k=0; k<this->contours.size(); k++) {
            if(this->contours[k].IsContour(objects[i])) {
                validContours.push_back(objects[i]);
                break;
            }
        }
    }
    
    //if maxContours is exceeded, filter out smaller contours and only take the bigger ones
    //basically, this block will go through the contours and test all of their sizes. 
    if(validContours.size() > maxContours) {
        std::vector<Contour> reducedValidContours = std::vector<Contour>();
        int contoursToBest = validContours.size() - maxContours; //contour must be bigger than at least this amount
        
        for(int i=0; i<validContours.size(); i++) {
            int area = validContours[i].Area();
            int numsGreaterThan = 0;

            //go though vector of valid contours and keep count of how many contours we are bigger than
            for(int k=0; k<validContours.size(); k++) {
                if(i != k && area > validContours[k].Area()) {
                    numsGreaterThan++;
                }
            }

            if(numsGreaterThan >= contoursToBest) {
                reducedValidContours.push_back(validContours[i]);
            }
        }

        validContours = reducedValidContours;
    }
    
    return validContours;
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

/**
 * Sets the value and allowable error of the property "prop" to the "contour"th contour of this target to the values specified in "values."
 * @param contour The ID of the contour to set.
 * @param prop The property to set.
 * @param values The values (value and allowable error) to set the property to.
 */
void ExampleTarget::SetContourProperty(int contour, TargetProperty prop, SettingPair values) {  
    //get the index of the contour we want to change, that way we directly set the values instead of taking a reference
        
    //make sure we have a contour with the id "contour"
    if(contour > this->contours.size()) {
        return;
    }

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

/**
 * Returns the value and allowable error of the property "prop" of the "contour"th contour of this ExampleTarget.
 * @param contour The ID of the contour to read the property from.
 * @param prop The property to read.
 */
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

/**
 * Sets the value of this targets "prop" to "value."
 * @param prop The property to set
 * @param value The value to set the property to.
 */
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
        case RunnerProperty::CALC_DIST_BY_HEIGHT:
            this->distMode = (value == 1 ? DistanceCalcMode::BY_HEIGHT : DistanceCalcMode::BY_WIDTH);
            break;
        case RunnerProperty::MAX_CONTOURS:
            this->maxContours = (int) value;
    }
}

/**
 * Reads the value of "prop" from this ExampleTarget.
 * @param prop The property to read.
 */
double ExampleTarget::GetTargetProperty(RunnerProperty prop) {
    double value = -1.0;

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
        case RunnerProperty::CALC_DIST_BY_HEIGHT:
            value = (this->distMode == DistanceCalcMode::BY_HEIGHT ? 1 : 0);
            break;
        case RunnerProperty::MAX_CONTOURS:
            value = (double) this->maxContours;
    }

    return value;
}

/**
 * Returns true if arr as any possible combos left based on the values "size" and "max," false otherwise.
 */
bool ExampleTarget::ArrayMaxed(int arr[], int size, int max) {
    if(arr[size - 1] < max) {
        return false;
    }

    return true;
}

/**
 * Returns true if arr contains duplicate values, false otherwise.
 */
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

/**
 * Evaluates whether "combo" matches any combonations present in "testedCombos."
 * @param combo An int array representing the combonation to be tested.
 * @param testedCombos A vector of combos which have already been tested.
 * @param comboSize The length of the combo array (numbers, not bytes! DO NOT use sizeof()!)
 */
bool ExampleTarget::CombonationAlreadyTested(int combo[], std::vector< std::vector<int> > testedCombos, int comboSize) {    
    for(int i=0; i<testedCombos.size(); i++) {
        std::vector<int> originalCombo = testedCombos[i];
        int matchingNumbers = 0;

        //compare all numbers between the two arrays
        for(int x=0; x<comboSize; x++) {
            int numberToTest = combo[x];

            for(int y=0; y<comboSize; y++) {
                int originalNumber = originalCombo[y];

                if(numberToTest == originalNumber) {
                    matchingNumbers++;
                }   
            }
        }

        //if all numbers in the combo match, then "combo" is not original, therefore return true.
        if(matchingNumbers == comboSize) {
            return true;
        }
    }

    return false;
}
