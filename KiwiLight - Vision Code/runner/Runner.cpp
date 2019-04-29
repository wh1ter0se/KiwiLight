#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


Runner::Runner(std::string fileName, bool debugging) {
    this->debug = debugging;
    this->postProcessorTargets = std::vector<ExampleTarget>();
    XMLDocument file = XMLDocument(fileName);
    if(file.HasContents()) {
        this->parseDocument(file);

        //use the information from XML file to init the UDP
        std::string udpAddr = this->settings.GetSetting("UDPAddress");
        int udpPort = std::stoi(this->settings.GetSetting("UDPPort"));
        this->udp = UDP(udpAddr, udpPort);

        //give some information to stdout about the config
        std::cout << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "KiwiLight Runner starting..." << std::endl;
        std::cout << "  Mode: " << (debugging? "Debug" : "Running") << std::endl;
        std::cout << "  Configuration Name: " << this->settings.GetSetting("configName") << std::endl;
        std::cout << "  Preprocessor: " << this->settings.GetSetting("PreprocessorType") << std::endl;
        std::cout << "  Postprocessor: full" << std::endl;
        std::cout << "    Number of Targets: " << this->postProcessorTargets.size() << std::endl;
        std::cout << "  UDP Destination Address: " << this->settings.GetSetting("UDPAddress") << std::endl;
        std::cout << "  UDP Port: " << this->settings.GetSetting("UDPPort") << std::endl;
        std::cout << "------------------------------------" << std::endl;

        std::cout << std::endl;
    } else {
        std::cout << "sorry! the file could not be found. " << std::endl;
    }

    this->stop = false;
}


void Runner::Loop() {
    int cameraIndex = std::stoi(this->settings.GetSetting("cameraIndex"));
    Camera cam = Camera(cameraIndex);

    //determine whether the preprocessor handles all image preprocessing or not
    bool isFull = (this->settings.GetSetting("PreprocessorType") == "full");

    //create preprocessor and then start!
    PreProcessor preprocessor = PreProcessor(this->settings, isFull);
    PostProcessor postprocessor = PostProcessor(this->postProcessorTargets);
    //loops a lot until stopped
    while(!stop) {
        cv::Mat img;
        if(RunnerSettings::USE_CAMERA) {
            img = cam.GetImageMat();
        } else {
            img = cv::imread("runner/dual.png");
        }
        cv::Mat preprocessed = preprocessor.ProcessImage(img);
        std::vector<Target> targets = postprocessor.ProcessImage(preprocessed);

        //find the percieved robot center using this->centerOffset
        int trueCenter = (std::stoi(this->settings.GetSetting("cameraWidth")) / 2);
        int robotCenter = trueCenter;
        //offset the true center here

        //find the target that is closest to the robot center
        Target bestTarget;
        int closestDist = 5000; // closest x distance to the center
        for(int i=0; i<targets.size(); i++) {
            Target targ = targets[i];
            int distFromCenter = robotCenter - targ.Center().x;
            distFromCenter = abs(distFromCenter);

            if(distFromCenter < closestDist) {
                closestDist = distFromCenter;
                bestTarget = targ;
            }
        }

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
        std::string x = std::to_string(coordX),
                        y = std::to_string(coordY),
                        d = std::to_string(distance),
                        a = std::to_string(angle);

        rioMessage = ":" + x + "," + y + "," + d + "," + a + ";";
        this->udp.Send(rioMessage);
        
        if(this->debug) {
            cv::Mat out;
            img.copyTo(out);

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

            std::string confName = this->settings.GetSetting("configName");
            cv::imshow(confName.c_str(), out);
            cv::waitKey(5);
        }
    }
}


void Runner::SetSetting(std::string settingName, std::string value) {
    this->settings.SetSetting(settingName, value);
}


std::string Runner::GetSetting(std::string settingName) {
    return this->settings.GetSetting(settingName);
}


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