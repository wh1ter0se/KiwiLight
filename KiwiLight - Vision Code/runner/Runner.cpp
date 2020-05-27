#include "../KiwiLight.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

const std::string Runner::NULL_MESSAGE = ":-1,-1,-1,-1,-1,180,180;";

/**
 * Creates a new runner which runs the configuration described by the given file
 */
Runner::Runner(std::string fileName, bool debugging) {
    this->src = fileName;
    this->debug = debugging;
    this->postProcessorTargets = std::vector<ExampleTarget>();
    this->lastIterationSuccessful = false;
    XMLDocument file = XMLDocument(fileName);
    if(file.HasContents()) {
        this->parseDocument(file);
    } else {
        std::cout << "sorry! the file " << fileName << " could not be found. " << std::endl;
    }
    this->applySettings(file);
    this->stop = false;
}

void Runner::SetImageResize(Size sz) {
    this->constantResize = sz;
}

/**
 * Performs one iteration of the main loop, but does not send any file UDP messages.
 * @return The message that should be sent to the RIO.
 */
std::string Runner::Iterate() {
    cv::Mat img;
    cv::Mat out; //output image we draw on for debugging
    if(RunnerSettings::USE_CAMERA) {
        img = KiwiLightApp::TakeImage();

        if(img.empty()) {
            //oops we shall exit now because there be nothing in image
            return NULL_MESSAGE;
        }
    } else {
        this->lastIterationSuccessful = true;
        img = cv::imread(RunnerSettings::IMAGE_TO_USE);
    }
    
    resize(img, img, this->constantResize);
    img.copyTo(this->originalImage);
    img = this->preprocessor.ProcessImage(img);
    img.copyTo(out);
    std::vector<Target> targets = this->postprocessor.ProcessImage(img);
    //find the percieved robot center using this->centerOffset
    int trueCenterX = (this->constantResize.width / 2);
    int robotCenterX = trueCenterX;

    int trueCenterY = (this->constantResize.height / 2);
    int robotCenterY = trueCenterY;

    //find the target that is closest to the robot center
    Target bestTarget;
    int closestDist = 5000; // closest horizontal distance to the center
    for(int i=0; i<targets.size(); i++) {
        Target targ = targets[i];

        //find the offset center of the camera based on the target's distance
        double inchesPerPixel = targ.KnownWidth() / targ.Bounds().width;
        double centerInchesX = trueCenterX * inchesPerPixel;
        double centerInchesY = trueCenterY * inchesPerPixel;

        double offsetInchesX = centerInchesX - this->centerOffsetX;
        double offestInchesY = centerInchesY - this->centerOffsetY;
    
        //convert back to pixels
        double offsetPixelsX = offsetInchesX / inchesPerPixel;
        double offsetPixelsY = offestInchesY / inchesPerPixel;

        //calculate the distance from the offset center
        double distFromCenterX = offsetPixelsX - targ.Center().x;
        double distFromCenterY = offsetPixelsY - targ.Center().y;
        double trueDistance = sqrt(pow(distFromCenterX, 2) + pow(distFromCenterY, 2));

        if(trueDistance < closestDist) {
            closestDist = trueDistance;
            bestTarget = targ;
            robotCenterX = (int) offsetPixelsX;
            robotCenterY = (int) offsetPixelsY;
        }
    }

    this->closestTarget = bestTarget;
    this->lastFrameCenterPoint = Point(robotCenterX, robotCenterY);

    //figure out which target to send and then send the target
    int coordX   = -1,
        coordY   = -1,
        width    = -1,
        height   = -1,
        distance = -1,
        HAngle   = 180,
        VAngle   = 180;

    std::string rioMessage = "";
    
    if(targets.size() > 0) {
        //use the best target to fill in the information to send to the rio
        coordX = bestTarget.Center().x;
        coordY = bestTarget.Center().y;
        
        width = bestTarget.Bounds().width;
        height = bestTarget.Bounds().height;

        distance = bestTarget.Distance();

        HAngle = bestTarget.HorizontalAngle(distance, robotCenterX);
        VAngle = bestTarget.VerticalAngle(distance, robotCenterY);
    }

    this->lastFrameTargets = targets;

    std::string x = std::to_string(coordX),
                y = std::to_string(coordY),
                w = std::to_string(width),
                h = std::to_string(height),
                d = std::to_string(distance),
                ax = std::to_string(HAngle),
                ay = std::to_string(VAngle);

    rioMessage = ":" + x + "," + y + "," + w + "," + h +"," + d + "," + ax + "," + ay + ";";

    //mark up the image with some stuff for the programmers to look at :)
    if(this->debug) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);

        //write the out string onto the image
        cv::putText(out, rioMessage, cv::Point(5, 15), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,0,255), 2);
        
        //draw a line where the perceived horizontal robot center is
        int camHeight = this->constantResize.height;
        cv::Point HlineTopPoint    = cv::Point(robotCenterX, 0);
        cv::Point HlineBottomPoint = cv::Point(robotCenterX, camHeight);
        cv::line(out, HlineTopPoint, HlineBottomPoint, cv::Scalar(255,0,255));

        //draw another line where the vertical robot center is
        int camWidth = this->constantResize.width;
        cv::Point VlineTopPoint    = cv::Point(0, robotCenterY);
        cv::Point VlineBottomPoint = cv::Point(camWidth, robotCenterY);
        cv::line(out, VlineTopPoint, VlineBottomPoint, cv::Scalar(255, 0, 255));
        
        //draw a dot in the center of each valid contour
        std::vector<Contour> contoursFromFrame = this->postprocessor.GetContoursFromLastFrame();
        std::vector<Contour> validContours = this->postprocessor.GetValidContoursForTarget(contoursFromFrame);
        
        Target targ = Target(0, validContours, 0, 0, 0, 0, DistanceCalcMode::BY_WIDTH);
        rectangle(out, targ.Bounds(), Scalar(0, 0, 255), 3);
        
        for(int i=0; i<validContours.size(); i++) {
            cv::circle(out, validContours[i].Center(), 3, Scalar(0,255,0), 4);
        }

        for(int i=0; i<targets.size(); i++) {
            cv::rectangle(out, targets[i].Bounds(), cv::Scalar(255,0,0), 3);
            cv::circle(out, targets[i].Center(), 3, cv::Scalar(255,255,0), 4);
            
            //draw the id number of the target on the image
            std::string id = std::to_string(targets[i].ID());
            cv::Point textPoint = targets[i].Center();
            textPoint.x -= (targets[i].Bounds().width / 2);
            textPoint.y -= (targets[i].Bounds().height / 2);
            textPoint.x += 5;
            textPoint.y += 5;
            cv::putText(out, id, textPoint, cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,0,255), 2);
        }

        //draw a special dot in the center of the target for which we send data
        cv::circle(out, bestTarget.Center(), 2, cv::Scalar(0,255,255), 3);
        
        out.copyTo(this->outputImage);
    }

    return rioMessage;
}

/**
 * Returns the number of contours that make up the target.
 */
int Runner::GetNumberOfContours(int target) {
    return this->postprocessor.NumberOfContours(target);
}

/**
 * Returns the example target at the given id. Returns the 0th exampletarget if id is out of bounds.
 */
ExampleTarget Runner::GetExampleTargetByID(int id) {
    return this->postprocessor.GetExampleTargetByID(id);
}

/**
 * Sets the ExampleTarget that this runner is tasked with finding.
 */
void Runner::SetExampleTarget(int contourID, ExampleTarget target) {
    this->postprocessor.SetTarget(contourID, target);
}

/**
 * Sets a property of the Runner's PreProcessor.
 * @param prop The property to set.
 * @param value The value to set the property to.
 */
void Runner::SetPreprocessorProperty(PreProcessorProperty prop, double value) {
    if(this->debug) {
        this->preprocessor.SetProperty(prop, value);
    }
}

/**
 * Reads a property from the Runner's PreProcessor.
 * @param prop The property to read.
 * @return The value of the property.
 */
double Runner::GetPreprocessorProperty(PreProcessorProperty prop) {
    return this->preprocessor.GetProperty(prop);
}

/**
 * Sets a contour property on the Runner's PostProcessor's ExampleTarget.
 * @param contour The ID of the contour to set.
 * @param prop The property to set.
 * @param values The values (value and allowable error) to set the property to.
 */
void Runner::SetPostProcessorContourProperty(int contour, TargetProperty prop, SettingPair values) {
    if(this->debug) {
        this->postprocessor.SetTargetContourProperty(contour, prop, values);
    }
}

/**
 * Reads a contour property on the Runner's PostProcessor's ExampleTarget.
 * @param contour The ID of the contour to read.
 * @param prop The property to read.
 */
SettingPair Runner::GetPostProcessorContourProperty(int contour, TargetProperty prop) {
    return this->postprocessor.GetTargetContourProperty(contour, prop);
}

/**
 * Sets a property of the Runner.
 * @param prop The property to set.
 * @param value The value to set the property to.
 */
void Runner::SetRunnerProperty(RunnerProperty prop, double value) {
    switch(prop) {
        case RunnerProperty::OFFSET_X:
            this->centerOffsetX = value;
            break;
        case RunnerProperty::OFFSET_Y:
            this->centerOffsetY = value;
            break;
        case RunnerProperty::IMAGE_WIDTH:
            this->constantResize.width = (int) value;
            break;
        case RunnerProperty::IMAGE_HEIGHT:
            this->constantResize.height = (int) value;
            break;
        default:
            this->postprocessor.SetRunnerProperty(prop, value);
            break;
    }
}

/**
 * Reads a property from the Runner.
 * @param prop The property to read.
 * @return The value of the property.
 */
double Runner::GetRunnerProperty(RunnerProperty prop) {
    switch(prop) {
        case RunnerProperty::OFFSET_X:
            return this->centerOffsetX;
        case RunnerProperty::OFFSET_Y:
            return this->centerOffsetY;
        case RunnerProperty::IMAGE_WIDTH:
            return this->constantResize.width;
        case RunnerProperty::IMAGE_HEIGHT:
            return this->constantResize.height;
    }

    return this->postprocessor.GetRunnerProperty(prop);
}

/**
 * Parses the XMLdocument doc and initalizes all runner settings and variables.
 * @param doc The XMLDocument to read.
 */
void Runner::parseDocument(XMLDocument doc) {
    XMLTag camera = doc.GetTagsByName("camera")[0];
        this->cameraIndex = std::stoi(camera.GetAttributesByName("index")[0].Value());
        
        std::vector<XMLTag> camSettings = 
            camera.GetTagsByName("settings")[0]
            .GetTagsByName("setting");

    XMLTag config = doc.GetTagsByName("configuration")[0];
        this->configName = doc.GetTagsByName("configuration")[0].GetAttributesByName("name")[0].Value();

        XMLTag cameraOffset = config.GetTagsByName("cameraOffset")[0];
            this->centerOffsetX = std::stod(cameraOffset.GetTagsByName("horizontal")[0].Content());
            this->centerOffsetY = std::stod(cameraOffset.GetTagsByName("vertical")[0].Content());

        XMLTag constResize = config.GetTagsByName("constantResize")[0];
            int resizeX = std::stoi(constResize.GetTagsByName("width")[0].Content());
            int resizeY = std::stoi(constResize.GetTagsByName("height")[0].Content());
            this->constantResize = Size(resizeX, resizeY);

        XMLTag preprocess = config.GetTagsByName("preprocessor")[0];
            bool preprocessorTypeIsFull = (preprocess.GetAttributesByName("type")[0].Value() == "full" ? true : false);
            
            int preprocessorThreshold = std::stoi(preprocess.GetTagsByName("threshold")[0].Content());
            int preprocessorErosion = std::stoi(preprocess.GetTagsByName("erosion")[0].Content());
            int preprocessorDilation = std::stoi(preprocess.GetTagsByName("dilation")[0].Content());
            XMLTag color = preprocess.GetTagsByName("targetColor")[0];
                int error = std::stoi(color.GetAttributesByName("error")[0].Value());
                int h = std::stoi(color.GetTagsByName("h")[0].Content());
                int s = std::stoi(color.GetTagsByName("s")[0].Content());
                int v = std::stoi(color.GetTagsByName("v")[0].Content());
                Color preprocessorColor = Color(h, s, v, error, error, error);

        XMLTag postprocess = config.GetTagsByName("postprocessor")[0];
            XMLTag udp = postprocess.GetTagsByName("UDP")[0];
                std::string udpAddr = udp.GetTagsByName("address")[0].Content();
                int udpPort = std::stoi(udp.GetTagsByName("port")[0].Content());

            std::vector<XMLTag> targets = postprocess.GetTagsByName("target");
            for(int i=0; i<targets.size(); i++) {
                XMLTag targetTag = targets[i];
                std::vector<XMLTag> targContours = targetTag.GetTagsByName("contour");
                
                int targetId = std::stoi(targetTag.GetAttributesByName("id")[0].Value());
                std::vector<ExampleContour> contours;

                //find all contours and populate the vector
                for(int k=0; k<targContours.size(); k++) {
                    XMLTag contour = targContours[k];
                    int id = std::stoi(contour.GetAttributesByName("id")[0].Value());
                    double x = std::stod(contour.GetTagsByName("x")[0].Content());
                    double y = std::stod(contour.GetTagsByName("y")[0].Content());
                    double distXError = std::stod(contour.GetTagsByName("x")[0].GetAttributesByName("error")[0].Value());
                    double distYError = std::stod(contour.GetTagsByName("y")[0].GetAttributesByName("error")[0].Value());
                    int angle = std::stoi(contour.GetTagsByName("angle")[0].Content());
                    int angleError = std::stoi(contour.GetTagsByName("angle")[0].GetAttributesByName("error")[0].Value());
                    double solidity = std::stod(contour.GetTagsByName("solidity")[0].Content());
                    double solidError = std::stod(contour.GetTagsByName("solidity")[0].GetAttributesByName("error")[0].Value());
                    double ar = std::stod(contour.GetTagsByName("aspectRatio")[0].Content());
                    double arError = std::stod(contour.GetTagsByName("aspectRatio")[0].GetAttributesByName("error")[0].Value());
                    int minArea = std::stoi(contour.GetTagsByName("minimumArea")[0].Content());

                    SettingPair distXPair = SettingPair(x, distXError);
                    SettingPair distYPair = SettingPair(y, distYError);
                    SettingPair anglePair = SettingPair(angle, angleError);
                    SettingPair solidPair = SettingPair(solidity, solidError);
                    SettingPair arPair    = SettingPair(ar, arError);

                    ExampleContour newContour = ExampleContour(id, distXPair, distYPair, anglePair, arPair, solidPair, minArea);
                    contours.push_back(newContour);
                }

                //knownWidth, focalWidth, calibratedDistance, distErrorCorrect
                double knownWidth = std::stod(targetTag.GetTagsByName("knownWidth")[0].Content());
                double focalWidth = std::stod(targetTag.GetTagsByName("focalWidth")[0].Content());
                double calibratedDistance = std::stod(targetTag.GetTagsByName("calibratedDistance")[0].Content());
                double distErrorCorrect = std::stod(targetTag.GetTagsByName("distErrorCorrect")[0].Content());

                bool calcByHeight = targetTag.GetTagsByName("calcByHeight")[0].Content() == "true";
                DistanceCalcMode distMode = (calcByHeight ? DistanceCalcMode::BY_HEIGHT : DistanceCalcMode::BY_WIDTH);

                ExampleTarget newTarget = ExampleTarget(targetId, contours, knownWidth, focalWidth, distErrorCorrect, calibratedDistance, distMode);
                this->postProcessorTargets.push_back(newTarget);
            }

    //init the preprocessor and postprocessor here
    this->preprocessor = PreProcessor(preprocessorTypeIsFull, preprocessorColor, preprocessorThreshold, preprocessorErosion, preprocessorDilation, this->debug);
    this->postprocessor = PostProcessor(this->postProcessorTargets, this->debug);
    KiwiLightApp::ReconnectUDP(udpAddr, udpPort);
}

/**
 * Applies the camera settings via shell.
 * @param document The XMLDocument to read the settings from.
 */
void Runner::applySettings(XMLDocument document) {
    std::vector<XMLTag> camSettings =
        document.GetTagsByName("camera")[0]
        .GetTagsByName("settings")[0]
        .GetTagsByName("setting");
        
    int camIndex = std::stoi(document.GetTagsByName("camera")[0].GetAttributesByName("index")[0].Value());
    KiwiLightApp::OpenNewCameraOnIndex(camIndex);

    for(int i=0; i<camSettings.size(); i++) {
        XMLTag setting = camSettings[i];
        int settingID = std::stoi(setting.GetAttributesByName("id")[0].Value());
        double settingValue = std::stod(setting.Content());
        KiwiLightApp::SetCameraProperty(settingID, settingValue);
    }
}
