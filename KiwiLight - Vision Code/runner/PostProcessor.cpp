#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PostProcessor finding "targets."
 * DEPRECATED: PostProcessor is moving away from using multiple targets. this constructor is no longer needed.
 */
PostProcessor::PostProcessor(std::vector<ExampleTarget> targets, bool debugging) {
    this->debugging = debugging;
    this->target = targets[0];
}

/**
 * Creates a new PostProcessor finding "target"
 */
PostProcessor::PostProcessor(ExampleTarget target, bool debugging) {
    this->target = target;
    this->debugging = debugging;
}

/**
 * Sets the ExampleTarget that this PostProcessor is tasked with finding.
 * DEPRECATED: PostProcessor is moving away from using multiple targets. Use SetTarget(ExampleTarget) instead.
 */
void PostProcessor::SetTarget(int id, ExampleTarget target) {
    this->target = target;
}


void PostProcessor::SetTarget(ExampleTarget target) {
    this->target = target;
}

/**
 * Returns the number of ExampleTargets that this PostProcessor is tasked with finding.
 * DEPRECATED: PostProcessor is moving away from using multiple targets. this method is no longer needed.
 * This method will now always return 1.
 */
int PostProcessor::NumberOfTargets() {
    return 1;
}

/**
 * Returns the number of contours of this PostProcessor's target.
 * DEPRECATED: PostProcessor is moving away from using multiple targets. Use NumberOfContours() instead.
 */
int PostProcessor::NumberOfContours(int target) {
    return NumberOfContours();
}

/**
 * Returns the number of contours of the PostProcessor's target.
 */
int PostProcessor::NumberOfContours() {
    return target.Contours().size();
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

    std::vector<Target> targs = this->target.GetTargets(objects);

    // add results to our found targets 
    for(int a=0; a<targs.size(); a++) {
        foundTargets.push_back(targs[a]);
    }

    return foundTargets;
}

/**
 * Returns a vector containing only contours that have a possiblity of being in the target.
 */
std::vector<Contour> PostProcessor::GetValidContoursForTarget(std::vector<Contour> contours) {
    return this->target.GetValidContours(contours);
}

/**
 * Sets a property of the one of the target's contours.
 * @param contour The ID of the contour to set the property of.
 * @param prop The property to set.
 * @param values The values (value and allowable error) to set the property to.
 */
void PostProcessor::SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values) {
    if(this->debugging) {
        this->target.SetContourProperty(contour, prop, values);
    }
}

/**
 * Reads a property of one of the target's contours.
 * @param contour The ID of the contour to read from.
 * @param prop The property to read.
 */
SettingPair PostProcessor::GetTargetContourProperty(int contour, TargetProperty prop) {
    return this->target.GetContourProperty(contour, prop);
}

/**
 * Sets a property of the PostProcessor.
 * @param prop The property to set.
 * @param value The value to set the property to.
 */
void PostProcessor::SetRunnerProperty(RunnerProperty prop, double value) {
    this->target.SetTargetProperty(prop, value);
}

/**
 * Reads a property of the PostProcessor.
 * @param prop The property to read.
 */
double PostProcessor::GetRunnerProperty(RunnerProperty prop) {
    return this->target.GetTargetProperty(prop);
}

/**
 * Returns the ExampleTarget at id.
 * DEPRECATED: PostProcessor is moving away from using multiple targets. use GetTarget() instead.
 */
ExampleTarget PostProcessor::GetExampleTargetByID(int id) {
    return GetTarget();
}


ExampleTarget PostProcessor::GetTarget() {
    return target;
}