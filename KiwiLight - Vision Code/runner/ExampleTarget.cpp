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
std::vector<Target> ExampleTarget::GetTargets(std::vector<Contour> objects) {
    std::vector<Target> foundTargets = std::vector<Target>();
    int numContours = this->contours.size();

    //find every possible combination of contours to find any targets with the vector of targets
    for(int i=0; i<objects.size(); i++) {
        std::vector<Contour> potentialTarget = std::vector<Contour>();
        potentialTarget.push_back(objects[i]);

        if(isTarget(potentialTarget)) {
            Target newTarget = Target(this->id, potentialTarget);
            foundTargets.push_back(newTarget);
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
     * use the equally sized objects vector to find matching contours for this target.
     * when matching contours are found, they are removed.
     * if there is nothing left (all contours were matches) the method returns true,
     * if there are any contours left, return false
     */

    std::vector<Contour> potentialTarget = objects;
    std::vector<ExampleContour> target = this->contours; //create copy of this thing

    for(int i=0; i<potentialTarget.size(); i++) {
        for(int k=0; k<target.size(); k++) {

            if(target[k].IsContour(potentialTarget[i])) {
                //the object is part of the target, remove target at k and object at i
                std::vector<Contour> newPotentialTarget = std::vector<Contour>();
                std::vector<ExampleContour> newTarget = std::vector<ExampleContour>();
                for(int a=0; a<potentialTarget.size(); a++) {
                    if(a != i) {
                        //object is not the one we want to remove, add it to new vector
                        newPotentialTarget.push_back(potentialTarget[a]);
                    }
                }

                for(int b=0; b<target.size(); b++) {
                    if(b != k) {
                        newTarget.push_back(target[b]);
                    }
                }

                potentialTarget = newPotentialTarget;
                target = newTarget;
                i--;
                k--;
                break;
            }
        }
    }

    return target.size() == 0;
}