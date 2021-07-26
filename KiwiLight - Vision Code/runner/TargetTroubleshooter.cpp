#include "Runner.h"

/**
 * Source file for the TargetTroubleshooter class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

static const int TROUBLESHOOT_FRAMES = 50;

/**
 * Creates a new TargetTroubleshooter. The "target" parameter is the target that needs to be troubleshot.
 */
TargetTroubleshooter::TargetTroubleshooter(VideoCapture cap, PreProcessor preprocessor, ExampleTarget target) {
    this->cap = cap;
    this->preprocessor = preprocessor;
    this->target = target;
}

/**
 * Troubleshoots the target.
 * @param allowableMissedTests how many tests a contour is allowed to miss before it cannot be part of the target.
 * @param dataOut an array of TroubleshootingData that test results and values will be dumped in by the method.
 * Precondition: dataOut is the same size as target.Contours.size()
 * 
 * NOTE: If this class is to ever make its way back into KiwiLight, it must be changed to a "feeding" system!
 */
bool TargetTroubleshooter::Troubleshoot(TroubleshootingData dataOut[]) {
    CameraFrame frames[TROUBLESHOOT_FRAMES];
    int minimumArea = 50000;

    //figure out the minimum area
    for(int i=0; i<this->target.Contours().size(); i++) {
        if(this->target.Contours()[i].MinimumArea() < minimumArea) {
            minimumArea = this->target.Contours()[i].MinimumArea();
        }
    }

    //capture the frames
    for(int i=0; i<TROUBLESHOOT_FRAMES; i++) {
        Mat img;
        bool success = false;
        if(RunnerSettings::USE_CAMERA) {
            success = this->cap.read(img);
        } else {
            success = true;
            img = imread(RunnerSettings::IMAGE_TO_USE);
        }
        img = this->preprocessor.ProcessImage(img);

        if(success) {
            CameraFrame newFrame = CameraFrame(img, minimumArea);
            frames[i] = newFrame;
        } else {
            i--;
            std::cout << "CAMERA GRAB FAILED! ";
        }

        double percentDone = i / (double) TROUBLESHOOT_FRAMES;
        percentDone *= 100;

        this->outString = "Collecting and processing images (" + std::to_string((int) percentDone) + "%)";
    }

    //figure out the average number of contours per frame
    std::vector<double> avgContours;
    for(int i=0; i<TROUBLESHOOT_FRAMES; i++) {
        avgContours.push_back(frames[i].NumberOfContours());
    }

    int averageNumContours = (int) Util::Average(avgContours);

    //group contours by distance
    std::vector <std::vector <Contour> > groupedContours = std::vector< std::vector <Contour> >(this->target.Contours().size());

    int discardedFrames = 0;
    for(int i=0; i<TROUBLESHOOT_FRAMES; i++) {
        if(frames[i].NumberOfContours() == averageNumContours) {
            //returns the contours sorted by distances
            std::vector<Contour> grouped = frames[i].GetContoursGrouped();
            
            for(int a=0; a<grouped.size(); a++) {
                groupedContours[a].push_back(grouped[a]);
            }
        } else {
            discardedFrames++;
        }
    }

    //match the contours to example contours
    std::vector <std::vector <Contour> > matchedContours = std::vector <std::vector <Contour> >(this->target.Contours().size()); //index 0 of matchedContours will correspond with exampleContour with id 0, 1 with 1, 2 with 2, etc
    std::cout << "matched contours size: " << matchedContours.size() << std::endl;
    std::vector<ExampleContour> targetContours = this->target.Contours();

    for(int i=0; i<groupedContours.size(); i++) {
        std::vector<Contour> group = groupedContours[i];
        std::vector<double> horizontalDistances;
        std::vector<double> verticalDistances;

        std::cout << "group size: " << group.size() << std::endl;

        for(int a=0; a<group.size(); a++) {
            Distance distFromCenter = frames[a].GetContourDistance(group[a]);

            horizontalDistances.push_back(distFromCenter.x);
            verticalDistances.push_back(distFromCenter.y);
        }

        double averageHorizontalDistance = Util::Average(horizontalDistances);
        double averageVerticalDistance = Util::Average(verticalDistances);

        //pythagorean theorem
        double AHDSquared = pow(averageHorizontalDistance, 2); //a
        double AVDSquared = pow(averageVerticalDistance, 2); //b
        double c = AHDSquared + AVDSquared;
        double distance = sqrt(c);

        int closestExampleTarget = 0;
        double closestDistance = 50000.0;

        //find the closest exampleContour distance
        for(int b=0; b<targetContours.size(); b++) {
            ExampleContour contour = targetContours[b];

            //pythagorean theorem again
            double contourHD = pow(contour.DistX().Value(), 2);
            double contourVD = pow(contour.DistY().Value(), 2);

            double contourDistSquared = contourHD + contourVD;
            double contourDistance = sqrt(contourDistSquared);

            double distDifference = contourDistance - distance;
            if(distDifference < closestDistance) {
                closestExampleTarget = b;
                closestDistance = distDifference;
            }
        }

        std::cout << "targetContours size: " << targetContours.size() << std::endl;
        std::cout << "closest match: " << closestExampleTarget << std::endl;

        matchedContours[targetContours[closestExampleTarget].ID()] = group;
        targetContours.erase(targetContours.begin() + closestExampleTarget);
    }

    TroubleshootingData data[(const int) this->target.Contours().size()];

    //run tests on the contours and build the data in TroubleshootingData
    for(int i=0; i<matchedContours.size(); i++) {

        data[i].missedHorizontalDistances = 0;
        data[i].missedVerticalDistances = 0;
        data[i].missedAngles = 0;
        data[i].missedSolidities = 0;
        data[i].missedAspectRatios = 0;

        std::vector<double> HorizontalDistances;
        std::vector<double> VerticalDistances;
        std::vector<double> Angles;
        std::vector<double> Solidities;
        std::vector<double> AspectRatios;

        std::cout << "matched group " << i << " size: " << matchedContours[i].size() << std::endl;
        for(int a=0; a<matchedContours[i].size(); a++) {
            ExampleContour testingContour = this->target.Contours()[i];
            Contour contourToTest = matchedContours[i][a];

            //test contour
            Distance dist = frames[a].GetContourDistance(contourToTest);
            bool distXTest = (dist.x < testingContour.DistX().UpperBound()) && (dist.x > testingContour.DistX().LowerBound());
            bool distYTest = (dist.y < testingContour.DistY().UpperBound()) && (dist.y > testingContour.DistY().LowerBound());
            bool angleTest = (contourToTest.Angle() < testingContour.Angle().UpperBound()) && (contourToTest.Angle() > testingContour.Angle().LowerBound());
            bool solidityTest = (contourToTest.Solidity() < testingContour.Solidity().UpperBound()) && (contourToTest.Solidity() > testingContour.Solidity().LowerBound());
            bool aspectTest = (contourToTest.AspectRatio() < testingContour.AspectRatio().UpperBound()) && (contourToTest.AspectRatio() > testingContour.AspectRatio().LowerBound());

            HorizontalDistances.push_back(dist.x);
            VerticalDistances.push_back(dist.y);
            Angles.push_back(contourToTest.Angle());
            Solidities.push_back(contourToTest.Solidity());
            AspectRatios.push_back(contourToTest.AspectRatio());

            if(!distXTest) data[i].missedHorizontalDistances++;
            if(!distYTest) data[i].missedVerticalDistances++;
            if(!angleTest) data[i].missedAngles++;
            if(!solidityTest) data[i].missedSolidities++;
            if(!aspectTest) data[i].missedAspectRatios++;
        }

        std::cout << "horizontal distances: " << Util::VectorToString(HorizontalDistances) << std::endl;
        std::cout << "vertical distances: " << Util::VectorToString(VerticalDistances) << std::endl;
        std::cout << "angles: " << Util::VectorToString(Angles) << std::endl;
        std::cout << "solidities: " << Util::VectorToString(Solidities) << std::endl;
        std::cout << "aspect ratios: " << Util::VectorToString(AspectRatios) << std::endl;

        data[i].averageHorizontalDistance = Util::Average(HorizontalDistances);
        data[i].averageVerticalDistance = Util::Average(VerticalDistances);
        data[i].averageAngle = Util::Average(Angles);
        data[i].averageSolidity = Util::Average(Solidities);
        data[i].averageAspectRatio = Util::Average(AspectRatios);
    }

    std::cout << "TROUBLESHOOTING RESULTS:" << std::endl;
    std::cout << std::endl;

    for(int i=0; i<matchedContours.size(); i++) {
        std::cout << "Contour " << i << std::endl;
        std::cout << "Missed Horizontal Distances: " << data[i].missedHorizontalDistances << std::endl;
        std::cout << "Missed Vertical Distances: " << data[i].missedVerticalDistances << std::endl;
        std::cout << "Missed Angles: " << data[i].missedAngles << std::endl;
        std::cout << "Missed Solidities: " << data[i].missedSolidities << std::endl;
        std::cout << "Missed Aspect Ratios: " << data[i].missedAspectRatios << std::endl;
        std::cout << "Average Horizontal Distance: " << data[i].averageHorizontalDistance << std::endl;
        std::cout << "Average Vertical Distance: " << data[i].averageVerticalDistance << std::endl;
        std::cout << "Average Angle: " << data[i].averageAngle << std::endl;
        std::cout << "Average Solidity: " << data[i].averageSolidity << std::endl;
        std::cout << "Average Aspect Ratio: " << data[i].averageAspectRatio << std::endl;
        std::cout << std::endl;
    }

    dataOut = data;

    return true;
}
