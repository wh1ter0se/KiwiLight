#include "Runner.h"

/**
 * Source file for the ConfigLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


ConfigLearner::ConfigLearner(PreProcessor preprocessor) {
    this->preprocessor = preprocessor;
    this->currentlyLearning = false;
    this->failedFrames = 0;
}


void ConfigLearner::StartLearning() {
    this->currentlyLearning = true;
}

/**
 * Feeds an unprocessed image into the learner for possible processing.
 */
void ConfigLearner::FeedImage(Mat img, int minimumContourArea) {
    //if the camera grab has failed and the image doesn't exist...
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

        if(area > minimumContourArea) {
            bigContours.push_back(contour);
        }
    }
    
    if(bigContours.size() > 0) {
        std::vector<Contour> contourObjects;
        for(int i=0; i<bigContours.size(); i++) {
            Contour newObject = Contour(bigContours[i]);
            contourObjects.push_back(newObject);
        }

        Target targ = Target(0, contourObjects, 0.0, 0.0, 0.0, 0.0);

        rectangle(this->out, targ.Bounds(), Scalar(255, 0, 0), 3);
        circle(this->out, targ.Center(), 3, Scalar(0,255,0), 4);
    }

    //if the learner is learning we should make a camera frame and store it for further processing
    if(this->currentlyLearning) {
        CameraFrame newFrame = CameraFrame(preprocessed, minimumContourArea);
        this->currentFrames.push_back(newFrame);
    }
}


ExampleTarget ConfigLearner::StopLearning(int minimumContourArea) {
    std::cout << "sl1" << std::endl;
    this->currentlyLearning = false;

    std::cout << "sl2" << std::endl;

    //create a sorted list of the number of contours in each image (it is double because DataUtils sorts doubles)
    std::vector<double> numberContoursList;
    std::vector <std::vector <Contour> > groupedContours; //vector of contours grouped by their distance from target center
    for(int i=0; i<this->currentFrames.size(); i++) {
        if(currentFrames[i].GetContoursGrouped().size() > 0) {
            groupedContours.push_back(this->currentFrames[i].GetContoursGrouped());
            numberContoursList.push_back((double) this->currentFrames[i].NumberOfContours());
        }
    }

    std::cout << "sl3" << std::endl;

    double regularNumberOfContours = DataUtils::MostCommonValue(numberContoursList);

    std::cout << "sl4" << std::endl;

    //discard any frame that does not have the same contours as regularNumberOfContours
    for(int i=0; i<this->currentFrames.size(); i++) {
        if(this->currentFrames[i].NumberOfContours() != regularNumberOfContours) {
            this->currentFrames.erase(this->currentFrames.begin() + i);
            i--;
        }
    }

    std::cout << "sl5a" << std::endl;

    //go through groupedContours to get and format data.
    std::vector<ExampleContour> finishedContours;
    for(int i=0; i<regularNumberOfContours; i++) {
        std::cout << "sl5b" << std::endl;
        std::vector<double> horizontalDistances = std::vector<double>();
        std::vector<double> verticalDistances   = std::vector<double>();
        std::vector<double> angles              = std::vector<double>();
        std::vector<double> solidities          = std::vector<double>();
        std::vector<double> aspectRatios        = std::vector<double>();
        std::cout << "sl5c" << std::endl;

        for(int k=0; k<groupedContours.size(); k++) {
            //gather and format contour data
            std::cout << "sl5ca" << std::endl;

            std::cout << "grouped contours dimensions: " << k << " x " << i << std::endl;
            std::cout << "i: " << i << std::endl;
            std::cout << "k: " << k << std::endl;

            Contour contourToAnalyze = groupedContours[k][i];
            std::cout << "sl5cb" << std::endl;
            Distance contourDistToCenter = this->currentFrames[k].GetContourDistance(contourToAnalyze);

            std::cout << "sl5cc" << std::endl;

            horizontalDistances.push_back(contourDistToCenter.x);
            std::cout << "sl5cd" << std::endl;
            verticalDistances.push_back(contourDistToCenter.y);
            std::cout << "sl5ce" << std::endl;
            angles.push_back(contourToAnalyze.Angle());
            std::cout << "sl5cf" << std::endl;
            solidities.push_back(contourToAnalyze.Solidity());
            std::cout << "sl5cg" << std::endl;
            aspectRatios.push_back(contourToAnalyze.AspectRatio());
            std::cout << "sl5ch" << std::endl;
        }

        std::cout << "sl5d" << std::endl;

        horizontalDistances = DataUtils::SortLeastGreatestDouble(horizontalDistances);
        verticalDistances   = DataUtils::SortLeastGreatestDouble(verticalDistances);
        angles              = DataUtils::SortLeastGreatestDouble(angles);
        solidities          = DataUtils::SortLeastGreatestDouble(solidities);
        aspectRatios        = DataUtils::SortLeastGreatestDouble(aspectRatios);

        std::cout << "sl5e" << std::endl;

        //record the sizes of each array so we can see how many outliers are removed
        std::vector<double> dataSizes;
        dataSizes.push_back((double) horizontalDistances.size());
        dataSizes.push_back((double) verticalDistances.size());
        dataSizes.push_back((double) angles.size());
        dataSizes.push_back((double) solidities.size());
        dataSizes.push_back((double) aspectRatios.size());

        std::cout << "sl5f" << std::endl;

        //remove outliers
        horizontalDistances = DataUtils::RemoveOutliers(horizontalDistances, 0.5);
        verticalDistances   = DataUtils::RemoveOutliers(verticalDistances, 0.5);
        angles              = DataUtils::RemoveOutliers(angles, 10.0);
        solidities          = DataUtils::RemoveOutliers(solidities, 0.25);
        aspectRatios        = DataUtils::RemoveOutliers(aspectRatios, 0.25);

        std::cout << "sl5g" << std::endl;

        std::vector<double> newDataSizes;
        newDataSizes.push_back((double) horizontalDistances.size());
        newDataSizes.push_back((double) verticalDistances.size());
        newDataSizes.push_back((double) angles.size());
        newDataSizes.push_back((double) solidities.size());
        newDataSizes.push_back((double) aspectRatios.size());

        std::cout << "sl5h" << std::endl;

        double originalTotal = DataUtils::Total(dataSizes);
        double removedTotal = DataUtils::Total(newDataSizes);

        std::cout << "sl5i" << std::endl;

        double totalRemoved = originalTotal - removedTotal;
        double avgRemoved = totalRemoved / newDataSizes.size();

        std::cout << "sl5j" << std::endl;

        double averageHorizontalDistance = DataUtils::Average(horizontalDistances);
        double averageVerticaldistance   = DataUtils::Average(verticalDistances);
        double averageAngle              = DataUtils::Average(angles);
        double averageSolidity           = DataUtils::Average(solidities);
        double averageAspectRatio        = DataUtils::Average(aspectRatios);

        std::cout << "sl5k" << std::endl;

        SettingPair horizontalDistancePair = SettingPair(averageHorizontalDistance, 0.25);
        SettingPair verticalDistancePair   = SettingPair(averageVerticaldistance, 0.25);
        SettingPair anglePair              = SettingPair(averageAngle, 10.0);
        SettingPair solidityPair           = SettingPair(averageSolidity, 0.25);
        SettingPair aspectRatioPair        = SettingPair(averageAspectRatio, 0.25);

        std::cout << "sl5l" << std::endl;

        ExampleContour newExampleContour = ExampleContour(i, horizontalDistancePair, verticalDistancePair, anglePair, aspectRatioPair, solidityPair, minimumContourArea);
        finishedContours.push_back(newExampleContour);

        std::cout << "sl5m" << std::endl;

        double percentComplete = (((double) i / (double) regularNumberOfContours) * 10.0) + 90.0;

        std::cout << "sl5n" << std::endl;
    }

    std::cout << "sl6" << std::endl;

    ExampleTarget newTarget = ExampleTarget(0, finishedContours, 0.0, 0.0, 0.0, 0.0);

    std::cout << "sl7" << std::endl;

    //clear info from the last learn
    this->currentlyLearning = false;
    this->currentFrames = std::vector<CameraFrame>();
    std::cout << "sl8" << std::endl;
    return newTarget;
}


int ConfigLearner::GetFramesLearned() {
    return this->currentFrames.size();
}