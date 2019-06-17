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

    for(int k=0; k<this->targets.size(); k++) {
        std::vector<Target> targs = this->targets[k].GetTargets(objects);

        // std::cout << "targets: " << targs.size() << std::endl;

        // add results to our found targets 
        for(int a=0; a<targs.size(); a++) {
            foundTargets.push_back(targs[a]);
        }
    }

    return foundTargets;
}


void PostProcessor::SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values) {
    if(this->debugging) {
        this->targets[0].SetContourProperty(contour, prop, values);
    }
}


SettingPair PostProcessor::GetTargetContourProperty(int contour, TargetProperty prop) {
    return this->targets[0].GetContourProperty(contour, prop);
}


ExampleTarget PostProcessor::GetExampleTargetByID(int id) {
    for(int i=0; i<this->targets.size(); i++) {
        if(this->targets[i].ID() == id) {
            return this->targets[i];
        }
    }
    
    std::cout << "WARNING: Target at ID " << id << " does not exist!" << std::endl;
    return targets[0];
}


void PostProcessor::SetRunnerProperty(RunnerProperty prop, double value) {
    this->targets[0].SetTargetProperty(prop, value);
}


double PostProcessor::GetRunnerProperty(RunnerProperty prop) {
    return this->targets[0].GetTargetProperty(prop);
}