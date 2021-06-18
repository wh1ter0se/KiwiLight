#include "../KiwiLight.h"

/**
 * Source file for the ConfigLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new ConfigLearner which uses the passed preprocessor.
 */
ConfigLearner::ConfigLearner(PreProcessor preprocessor, Size imageSize) {
    this->preprocessor = preprocessor;
    this->imageSize = imageSize;
    this->minimumArea = 500;
    this->currentlyLearning = false;
    this->failedFrames = 0;
}

/**
 * Puts this ConfigLearner in a "learning" state, allowing it to be fed frames. No learning will be done unless this method is called first.
 */
void ConfigLearner::StartLearning() {
    this->currentlyLearning = true;
}

/**
 * Sets the minimum area that a contour must be to be considered to be part of the target being learned.
 */
void ConfigLearner::SetMinimumArea(int minimumArea) {
    this->minimumArea = minimumArea;
}

/**
 * Feeds an unprocessed image into the learner for processing.
 */
void ConfigLearner::Feed() {
    //if the camera grab has failed and the image doesn't exist...
    Mat img = KiwiLightApp::TakeImage();
    resize(img, img, imageSize);

    if(img.empty()) {
        failedFrames++;
        return;
    }

    img.copyTo(this->original);
    Mat preprocessed = this->preprocessor.ProcessImage(img);
    preprocessed.copyTo(this->out);

    cvtColor(this->out, this->out, cv::COLOR_GRAY2BGR);
    
    //find contours and highlight all contours that are bigger than a certain area
    std::vector <std::vector <Point> > contours; //all contours in image
    std::vector <std::vector <Point> > bigContours; // contours that are bigger than minArea
    findContours(preprocessed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //find all contours that are bigger than minimumArea, then highlight them.
    for(int i=0; i<contours.size(); i++) {
        std::vector<Point> contour = contours[i];

        Rect bounds = boundingRect(contour);
        int area = bounds.width * bounds.height;

        if(area > minimumArea) {
            bigContours.push_back(contour);
        }
    }
    
    if(bigContours.size() > 0) {
        std::vector<Contour> contourObjects;
        for(int i=0; i<bigContours.size(); i++) {
            Contour newObject = Contour(bigContours[i]);
            contourObjects.push_back(newObject);
        }

        Target targ = Target(0, contourObjects, 0.0, 0.0, 0.0, 0.0, DistanceCalcMode::BY_WIDTH);

        rectangle(this->out, targ.Bounds(), Scalar(255, 0, 0), 3);
        circle(this->out, targ.Center(), 3, Scalar(0,255,0), 4);
    }

    //if the learner is learning we should make a camera frame and store it for further processing
    if(this->currentlyLearning) {
        CameraFrame newFrame = CameraFrame(preprocessed, minimumArea);
        this->currentFrames.push_back(newFrame);
    }
}

/**
 * Disables this ConfigLearner's ability to be fed frames, and creates an ExampleTarget which represents the target which is being learned.
 * @return An ExampleTarget representing the "average" of all targets in all fed frames.
 */
ExampleTarget ConfigLearner::StopLearning() {
    this->currentlyLearning = false;

    //create a sorted list of the number of contours in each image (it is double because Util sorts doubles)
    std::vector<double> numberContoursList;
    for(int i=0; i<this->currentFrames.size(); i++) {
        numberContoursList.push_back(this->currentFrames[i].GetContoursGrouped().size());
    }

    double regularNumberOfContours = Util::MostCommonValue(numberContoursList);

    std::vector <std::vector <Contour> > groupedContours; //vector of contours grouped by their distance from target center
    for(int i=0; i<this->currentFrames.size(); i++) {
        if(currentFrames[i].GetContoursGrouped().size() == regularNumberOfContours) {
            groupedContours.push_back(this->currentFrames[i].GetContoursGrouped());
            numberContoursList.push_back((double) this->currentFrames[i].NumberOfContours());
        }
    }

    //go through groupedContours to get and format data.
    std::vector<ExampleContour> finishedContours;
    for(int i=0; i<regularNumberOfContours; i++) {
        std::vector<double> horizontalDistances = std::vector<double>();
        std::vector<double> verticalDistances   = std::vector<double>();
        std::vector<double> angles              = std::vector<double>();
        std::vector<double> solidities          = std::vector<double>();
        std::vector<double> aspectRatios        = std::vector<double>();

        for(int k=0; k<groupedContours.size(); k++) {
            //gather and format contour data
            Contour contourToAnalyze = groupedContours[k][i];
            Distance contourDistToCenter = this->currentFrames[k].GetContourDistance(contourToAnalyze);

            horizontalDistances.push_back(contourDistToCenter.x);
            verticalDistances.push_back(contourDistToCenter.y);
            angles.push_back(contourToAnalyze.Angle());
            solidities.push_back(contourToAnalyze.Solidity());
            aspectRatios.push_back(contourToAnalyze.AspectRatio());
        }

        horizontalDistances = Util::SortLeastGreatestDouble(horizontalDistances);
        verticalDistances   = Util::SortLeastGreatestDouble(verticalDistances);
        angles              = Util::SortLeastGreatestDouble(angles);
        solidities          = Util::SortLeastGreatestDouble(solidities);
        aspectRatios        = Util::SortLeastGreatestDouble(aspectRatios);

        //record the sizes of each array so we can see how many outliers are removed
        std::vector<double> dataSizes;
        dataSizes.push_back((double) horizontalDistances.size());
        dataSizes.push_back((double) verticalDistances.size());
        dataSizes.push_back((double) angles.size());
        dataSizes.push_back((double) solidities.size());
        dataSizes.push_back((double) aspectRatios.size());

        //remove outliers
        horizontalDistances = Util::RemoveOutliers(horizontalDistances, 0.5);
        verticalDistances   = Util::RemoveOutliers(verticalDistances, 0.5);
        angles              = Util::RemoveOutliers(angles, 10.0);
        solidities          = Util::RemoveOutliers(solidities, 0.25);
        aspectRatios        = Util::RemoveOutliers(aspectRatios, 0.25);

        std::vector<double> newDataSizes;
        newDataSizes.push_back((double) horizontalDistances.size());
        newDataSizes.push_back((double) verticalDistances.size());
        newDataSizes.push_back((double) angles.size());
        newDataSizes.push_back((double) solidities.size());
        newDataSizes.push_back((double) aspectRatios.size());

        double originalTotal = Util::Total(dataSizes);
        double removedTotal = Util::Total(newDataSizes);

        double totalRemoved = originalTotal - removedTotal;
        double avgRemoved = totalRemoved / newDataSizes.size();

        double averageHorizontalDistance = Util::Average(horizontalDistances);
        double averageVerticaldistance   = Util::Average(verticalDistances);
        double averageAngle              = Util::Average(angles);
        double averageSolidity           = Util::Average(solidities);
        double averageAspectRatio        = Util::Average(aspectRatios);

        SettingPair horizontalDistancePair = SettingPair(averageHorizontalDistance, 0.25);
        SettingPair verticalDistancePair   = SettingPair(averageVerticaldistance, 0.25);
        SettingPair anglePair              = SettingPair(averageAngle, 10.0);
        SettingPair solidityPair           = SettingPair(averageSolidity, 0.25);
        SettingPair aspectRatioPair        = SettingPair(averageAspectRatio, 0.25);

        ExampleContour newExampleContour = ExampleContour(i, horizontalDistancePair, verticalDistancePair, anglePair, aspectRatioPair, solidityPair, minimumArea);
        finishedContours.push_back(newExampleContour);
    }

    ExampleTarget newTarget = ExampleTarget(0, finishedContours, 0.0, 0.0, 0.0, 0.0, DistanceCalcMode::BY_WIDTH, 5);

    //clear info from the last learn
    this->currentlyLearning = false;
    this->currentFrames = std::vector<CameraFrame>();
    return newTarget;
}

/**
 * Returns the number of frames that have been "fed" to this ConfigLearner.
 */
int ConfigLearner::GetFramesLearned() {
    return this->currentFrames.size();
}