#include "Runner.h"

/**
 * Source file for the TargetDistanceLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


TargetDistanceLearner::TargetDistanceLearner(PreProcessor preprocessor, PostProcessor postprocessor) {
    this->preprocessor = preprocessor;
    this->postprocessor = postprocessor;
    this->frames = 0;
}


void TargetDistanceLearner::FeedImage(Mat image) {
    if(image.empty()) {
        failedFrames++;
        return;
    }
    
    Mat img(image);
    img = this->preprocessor.ProcessImage(img);
    std::vector<Target> frameTargs = this->postprocessor.ProcessImage(img);
    if(frameTargs.size() == 1) {
        Target targ = frameTargs[0];
        this->targetWidths.push_back((double) targ.Bounds().width);
        this->frames++;
    }
}


void TargetDistanceLearner::FeedTarget(Target targ) {
    this->targetWidths.push_back((double) targ.Bounds().width);
    this->frames++;
}


double TargetDistanceLearner::GetFocalWidth(double trueDistance, double trueWidth) {
    int sizeBeforeRemove = targetWidths.size();
    this->targetWidths = DataUtils::SortLeastGreatestDouble(this->targetWidths);
    this->targetWidths = DataUtils::RemoveOutliers(this->targetWidths, 30);
    
    double avgWidth = DataUtils::Average(targetWidths);
    
    double focalWidth = avgWidth * trueDistance / trueWidth;
    this->targetWidths = std::vector<double>(); //reset the vector so it can learn again
    this->frames = 0; //reset the frames counter so it can learn again
    return focalWidth;
}


int TargetDistanceLearner::GetFramesLearned() {
    return this->frames;
}
