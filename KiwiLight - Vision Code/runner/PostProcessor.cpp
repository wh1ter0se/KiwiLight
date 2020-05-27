#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PostProcessor with the given settings. 
 * Just give it all the runner settings, PostProcessor will pick out the ones it will use.
 */
PostProcessor::PostProcessor(std::vector<ExampleTarget> targets, bool debugging) {
    this->debugging = debugging;
    this->targets = targets;
    ExampleTarget targ = this->targets[0];
}

/**
 * Sets the ExampleTarget that this PostProcessor is tasked with finding.
 */
void PostProcessor::SetTarget(int id, ExampleTarget target) {
    //find the id to replace
    for(int i=0; i<this->targets.size(); i++) {
        if(this->targets[i].ID() == id) {
            this->targets[i] = target;
            return;
        }
    }
}

/**
 * Returns the number of contours of this PostProcessor's target.
 */
int PostProcessor::NumberOfContours(int target) {
    //find the target with the id, and return its contour count
    for(int i=0; i<targets.size(); i++) {
        if(this->targets[i].ID() == target) {
            return this->targets[i].Contours().size();
        }
    }

    return -1;
}

/**
 * Processes the given image (from the preprocessor) and returns a vector containing any 
 * targets it finds.
 */
std::vector<Target> PostProcessor::ProcessImage(cv::Mat img) {
    std::vector<Target> foundTargets = std::vector<Target>();

    //find contours with input image
    std::vector< std::vector< Point > > contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //create contour wrappers and prepare to compare with targets
    std::vector<Contour> objects = std::vector<Contour>();
    for(int i=0; i<contours.size(); i++) {
        Contour newContour = Contour(contours[i]);
        objects.push_back(newContour);
    }
    
    this->contoursFromLastFrame = objects;

    for(int k=0; k<this->targets.size(); k++) {
        std::vector<Target> targs = this->targets[k].GetTargets(objects);

        // add results to our found targets 
        for(int a=0; a<targs.size(); a++) {
            foundTargets.push_back(targs[a]);
        }
    }

    return foundTargets;
}

/**
 * Returns a vector containing only contours that have a possiblity of being in the target.
 */
std::vector<Contour> PostProcessor::GetValidContoursForTarget(std::vector<Contour> contours) {
    return this->targets[0].GetValidContours(contours);
}

/**
 * Sets a property of the one of the target's contours.
 * @param contour The ID of the contour to set the property of.
 * @param prop The property to set.
 * @param values The values (value and allowable error) to set the property to.
 */
void PostProcessor::SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values) {
    if(this->debugging) {
        this->targets[0].SetContourProperty(contour, prop, values);
    }
}

/**
 * Reads a property of one of the target's contours.
 * @param contour The ID of the contour to read from.
 * @param prop The property to read.
 */
SettingPair PostProcessor::GetTargetContourProperty(int contour, TargetProperty prop) {
    return this->targets[0].GetContourProperty(contour, prop);
}

/**
 * Returns the ExampleTarget at id.
 */
ExampleTarget PostProcessor::GetExampleTargetByID(int id) {
    for(int i=0; i<this->targets.size(); i++) {
        if(this->targets[i].ID() == id) {
            return this->targets[i];
        }
    }
    
    std::cout << "WARNING: Target at ID " << id << " does not exist!" << std::endl;
    return targets[0];
}

/**
 * Sets a property of the PostProcessor.
 * @param prop The property to set.
 * @param value The value to set the property to.
 */
void PostProcessor::SetRunnerProperty(RunnerProperty prop, double value) {
    this->targets[0].SetTargetProperty(prop, value);
}

/**
 * Reads a property of the PostProcessor.
 * @param prop The property to read.
 */
double PostProcessor::GetRunnerProperty(RunnerProperty prop) {
    return this->targets[0].GetTargetProperty(prop);
}
