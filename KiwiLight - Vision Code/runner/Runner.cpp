#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


Runner::Runner(std::string fileName, bool debugging) {
    this->debug = debugging;
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
    //loops a lot until stopped
    while(!stop) {
        cv::Mat img = cam.GetImage();
        cv::Mat preprocessed = preprocessor.ProcessImage(img);
        
        if(this->debug) {
            std::string confName = this->settings.GetSetting("confName");
            cv::imshow(confName.c_str(), preprocessed);
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
        
}