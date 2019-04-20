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
            img = cam.GetImage();
        } else {
            img = cv::imread("runner/runner_test_target.png");
        }
        cv::Mat preprocessed = preprocessor.ProcessImage(img);
        std::vector<Target> targets = postprocessor.ProcessImage(preprocessed);
        
        if(this->debug) {
            cv::Mat out;
            img.copyTo(out);

            for(int i=0; i<targets.size(); i++) {
                cv::rectangle(out, targets[i].Bounds(), cv::Scalar(255,0,0), 3);
                cv::circle(out, targets[i].Center(), 3, cv::Scalar(255,255,0), 4);
            }
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
                    int x = std::stoi(contour.GetTagsByName("x")[0].Content());
                    int y = std::stoi(contour.GetTagsByName("y")[0].Content());
                    int distError = 50;
                    Distance distFromCenter = Distance(x, y, distError);

                    int width = std::stoi(contour.GetTagsByName("width")[0].Content());
                    int height = std::stoi(contour.GetTagsByName("height")[0].Content());
                    int angle = std::stoi(contour.GetTagsByName("angle")[0].Content());
                    int solidity = std::stod(contour.GetTagsByName("solidity")[0].Content());

                    ExampleContour newContour = ExampleContour(id, distFromCenter, width, height, angle, solidity);
                    contours.push_back(newContour);
                }

                ExampleTarget newTarget = ExampleTarget(targetId, contours);
                this->postProcessorTargets.push_back(newTarget);
            }
}