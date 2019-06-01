#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new runner which runs the configuration described by the given file
 */
Runner::Runner(std::string fileName, bool debugging) {
    this->src = fileName;
    this->debug = debugging;
    this->postProcessorTargets = std::vector<ExampleTarget>();
    XMLDocument file = XMLDocument(fileName);
    if(file.HasContents()) {
        this->parseDocument(file);
    } else {
        std::cout << "sorry! the file could not be found. " << std::endl;
    }
    //figure out constant resize
    int resizeX = std::stoi(this->settings.GetSetting("resizeX"));
    int resizeY = std::stoi(this->settings.GetSetting("resizeY"));
    this->constantResize = Size(resizeX, resizeY);

    bool isFull = (this->settings.GetSetting("PreprocessorType") == "full");
    this->cameraIndex = std::stoi(this->settings.GetSetting("cameraIndex"));
    this->preprocessor = PreProcessor(this->settings, isFull, this->debug);
    this->postprocessor = PostProcessor(this->postProcessorTargets, this->debug);
    this->cap = VideoCapture(this->cameraIndex);
    this->stop = false;
}


Runner::Runner(std::string fileName, bool debugging, bool openNewVideoStream) {
    this->src = fileName;
    this->debug = debugging;
    this->postProcessorTargets = std::vector<ExampleTarget>();
    XMLDocument file = XMLDocument(fileName);
    if(file.HasContents()) {
        this->parseDocument(file);
    } else {
        std::cout << "sorry! the file could not be found. " << std::endl;
    }
    //figure out constant resize
    int resizeX = std::stoi(this->settings.GetSetting("resizeX"));
    int resizeY = std::stoi(this->settings.GetSetting("resizeY"));
    this->constantResize = Size(resizeX, resizeY);

    bool isFull = (this->settings.GetSetting("PreprocessorType") == "full");
    this->cameraIndex = std::stoi(this->settings.GetSetting("cameraIndex"));
    this->preprocessor = PreProcessor(this->settings, isFull, this->debug);
    this->postprocessor = PostProcessor(this->postProcessorTargets, this->debug);
    if(openNewVideoStream) {
        this->cap = VideoCapture(this->cameraIndex);
    }
    this->stop = false;
}


void Runner::SetImageResize(Size sz) {
    this->constantResize = sz;
}

/**
 * Puts the runner in a constant loop, and sends finished UDP messages.
 * This method would be used in normal FRC situations
 */
void Runner::Loop() {
    //give some information to stdout about the config
    std::cout << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "KiwiLight Runner starting..." << std::endl;
    std::cout << "  Mode: " << (this->debug? "Debug" : "Running") << std::endl;
    std::cout << "  Configuration Name: " << this->settings.GetSetting("configName") << std::endl;
    std::cout << "  Preprocessor: " << this->settings.GetSetting("PreprocessorType") << std::endl;
    std::cout << "  Postprocessor: full" << std::endl;
    std::cout << "    Number of Targets: " << this->postProcessorTargets.size() << std::endl;
    std::cout << "  UDP Destination Address: " << this->settings.GetSetting("UDPAddress") << std::endl;
    std::cout << "  UDP Port: " << this->settings.GetSetting("UDPPort") << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << std::endl;

     //use the information from XML file to init the UDP
    std::string udpAddr = this->settings.GetSetting("UDPAddress");
    int udpPort = std::stoi(this->settings.GetSetting("UDPPort"));
    UDP udp = UDP(udpAddr, udpPort);

    //loops a lot until stopped
    while(!stop) {
        //run algorithm and get the udp message to send to rio
        std::string rioString = Iterate();
        udp.Send(rioString);
    }
}

/**
 * Performs one iteration of the main loop, but does not send any file UDP messages.
 */
std::string Runner::Iterate() {
    cv::Mat img;
    cv::Mat out; //output image we draw on for debugging
    if(RunnerSettings::USE_CAMERA) {
        bool success = this->cap.read(img);
        if(!success) {
            //print out a message and exit the method call
            std::cout << "CAMERA GRAB FAILED!! ";
            return "";
        }
    } else {
        img = cv::imread("runner/dual.png");
    }

    resize(img, img, this->constantResize);
    img.copyTo(this->originalImage);
    img = this->preprocessor.ProcessImage(img);
    img.copyTo(out);
    std::vector<Target> targets = this->postprocessor.ProcessImage(img);
    //find the percieved robot center using this->centerOffset
    int trueCenter = (this->constantResize.width / 2);
    int robotCenter = trueCenter;
    //offset the true center here

    //find the target that is closest to the robot center
    Target bestTarget;
    int closestDist = 5000; // closest horizontal distance to the center
    for(int i=0; i<targets.size(); i++) {
        Target targ = targets[i];
        int distFromCenter = robotCenter - targ.Center().x;
        distFromCenter = abs(distFromCenter);

        if(distFromCenter < closestDist) {
            closestDist = distFromCenter;
            bestTarget = targ;
        }
    }

    this->closestTarget = bestTarget;

    //figure out which target to send and then send the target
    int coordX = -1,
        coordY = -1,
        distance = -1,
        angle = 180;

    std::string rioMessage = "";
    
    if(targets.size() > 0) {
        //use the best target to fill in the information to send to the rio
        coordX = bestTarget.Center().x;
        coordY = bestTarget.Center().y;

        distance = bestTarget.Distance();

        angle = bestTarget.Angle(distance, robotCenter);
    }

    this->lastFrameTargets = targets;

    std::string x = std::to_string(coordX),
                    y = std::to_string(coordY),
                    d = std::to_string(distance),
                    a = std::to_string(angle);

    rioMessage = ":" + x + "," + y + "," + d + "," + a + ";";

    if(this->debug) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2RGB);

        //write the out string onto the image
        cv::putText(out, rioMessage, cv::Point(5, 15), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0,0,255), 2);
        
        //draw a line where the perceived robot center is
        int camHeight = std::stoi(this->settings.GetSetting("cameraHeight"));
        cv::Point lineTopPoint    = cv::Point(robotCenter, 0);
        cv::Point lineBottomPoint = cv::Point(robotCenter, camHeight);

        cv::line(out, lineTopPoint, lineBottomPoint, cv::Scalar(255,0,255));

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
 * Returns the example target at the given id. Returns the 0th exampletarget if id is out of bounds.
 */
ExampleTarget Runner::GetExampleTargetByID(int id) {
    for(int i=0; i<this->postProcessorTargets.size(); i++) {
        if(postProcessorTargets[i].ID() == id) {
            return postProcessorTargets[i];
        }
    }
    std::cout << "WARNING: Target at ID " << id << " does not exist!" << std::endl;
    return postProcessorTargets[0];
}

/**
 * Stops runner and releases camera. Camera function will not be granted to anything else
 * unless this method is called.
 */
void Runner::Stop() {
    this->cap.~VideoCapture();
    this->stop = true;
}

/**
 * Stops the loop if one is happening, but does not close the camera stream.
 */
void Runner::StopLoopOnly() {
    this->stop = true;
}

/**
 * If Stop() or StopLoopOnly() has been called to stop the main loop, UnlockLoop() must be called
 * to allow a new loop to start.
 */
void Runner::UnlockLoop() {
    this->stop = false;
}

/**
 * Restarts the camera stream associated with this runner.
 * This should only be called if Stop() has been called.
 * NOTE: THIS METHOD DOES NOT CALL LOOP(). Loop() must be called separately.
 */
void Runner::Start() {
    int cameraIndex = std::stoi(this->settings.GetSetting("cameraIndex"));
    this->cap = VideoCapture(cameraIndex);
    this->stop = false;
}


void Runner::SetPreprocessorProperty(PreProcessorProperty prop, double value) {
    if(this->debug) {
        this->preprocessor.SetProperty(prop, value);
    }
}


double Runner::GetPreprocessorProperty(PreProcessorProperty prop) {
    return this->preprocessor.GetProperty(prop);
}


void Runner::SetPostProcessorContourProperty(int contour, TargetProperty prop, SettingPair values) {
    if(this->debug) {
        this->postprocessor.SetTargetContourProperty(contour, prop, values);
    }
}


SettingPair Runner::GetPostProcessorContourProperty(int contour, TargetProperty prop) {
    return this->postprocessor.GetTargetContourProperty(contour, prop);
}


void Runner::SetRunnerProperty(RunnerProperty prop, double value) {
    this->postprocessor.SetRunnerProperty(prop, value);
}


double Runner::GetRunnerProperty(RunnerProperty prop) {
    return this->postprocessor.GetRunnerProperty(prop);
}

/**
 * Returns the value of the settings at settingName.
 */
std::string Runner::GetSetting(std::string settingName) {
    return this->settings.GetSetting(settingName);
}

/**
 * Parses the XMLdocument doc and initalizes all runner settings and variables.
 */
void Runner::parseDocument(XMLDocument doc) {
    this->settings = ConfigurationSettingsList();

    XMLTag camera = doc.GetTagsByName("camera")[0];
        this->settings.AddSetting("cameraIndex", camera.GetAttributesByName("index")[0].Value());
        XMLTag camRes = camera.GetTagsByName("resolution")[0];
            this->settings.AddSetting("cameraWidth", camRes.GetTagsByName("width")[0].Content());
            this->settings.AddSetting("cameraHeight", camRes.GetTagsByName("height")[0].Content());

        XMLTag camSettings = camera.GetTagsByName("settings")[0];
            //do things for the settings here
    XMLTag config = doc.GetTagsByName("configuration")[0];
        this->settings.AddSetting("configName", config.GetAttributesByName("name")[0].Value());

        XMLTag constResize = config.GetTagsByName("constantResize")[0];
            this->settings.AddSetting("resizeX", constResize.GetTagsByName("width")[0].Content());
            this->settings.AddSetting("resizeY", constResize.GetTagsByName("height")[0].Content());

        XMLTag preprocess = config.GetTagsByName("preprocessor")[0];
            this->settings.AddSetting("PreprocessorType", preprocess.GetAttributesByName("type")[0].Value());
            XMLTag threshold = preprocess.GetTagsByName("targetThreshold")[0];
                this->settings.AddSetting("thresholdValue", threshold.GetTagsByName("threshold")[0].Content());
                this->settings.AddSetting("threshMaxValue", threshold.GetTagsByName("maxValue")[0].Content());
                this->settings.AddSetting("threshType", threshold.GetTagsByName("type")[0].Content());

            this->settings.AddSetting("dilation", preprocess.GetTagsByName("dilation")[0].Content());
            XMLTag color = preprocess.GetTagsByName("targetColor")[0];
                this->settings.AddSetting("colorH", color.GetTagsByName("h")[0].Content());
                this->settings.AddSetting("colorS", color.GetTagsByName("s")[0].Content());
                this->settings.AddSetting("colorV", color.GetTagsByName("v")[0].Content());
                this->settings.AddSetting("colorH_error", color.GetTagsByName("h")[0].GetAttributesByName("error")[0].Value());
                this->settings.AddSetting("colorS_error", color.GetTagsByName("s")[0].GetAttributesByName("error")[0].Value());
                this->settings.AddSetting("colorV_error", color.GetTagsByName("v")[0].GetAttributesByName("error")[0].Value());
        XMLTag postprocess = config.GetTagsByName("postprocessor")[0];
            this->settings.AddSetting("centerOffset", postprocess.GetTagsByName("centerOffset")[0].Content());
            this->centerOffset = std::stoi(this->settings.GetSetting("centerOffset"));

            XMLTag udp = postprocess.GetTagsByName("UDP")[0];
                this->settings.AddSetting("UDPAddress", udp.GetTagsByName("address")[0].Content());
                this->settings.AddSetting("UDPPort", udp.GetTagsByName("port")[0].Content());

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

                ExampleTarget newTarget = ExampleTarget(targetId, contours, knownWidth, focalWidth, distErrorCorrect, calibratedDistance);
                this->postProcessorTargets.push_back(newTarget);
            }
}