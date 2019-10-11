#include "../KiwiLight.h"

/**
 * Source file for the Settings class
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Create a new Settings menu.
 */
Settings::Settings(VideoCapture cap, XMLDocument doc) {

    std::vector<XMLTag> docSettings = 
        doc.GetTagsByName("camera")[0]
        .GetTagsByName("settings")[0]
        .GetTagsByName("setting");

    Panel editor = Panel(false, 0);

        //resolution area
        Label resolutionHeader = Label("Resolution");
            resolutionHeader.SetName("subHeader");
            editor.Pack_start(resolutionHeader.GetWidget(), true, true, 0);

        //frame width
        int realFrameWidth = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_FRAME_WIDTH).Content());
        cap.set(CAP_PROP_FRAME_WIDTH, (double) realFrameWidth);
        CameraSetting frameWidth = CameraSetting("Width: ", CAP_PROP_FRAME_WIDTH, 100, 2000, realFrameWidth);
        this->settings.push_back(frameWidth);
            editor.Pack_start(frameWidth.GetWidget(), true, true, 0);

        //frame height
        int realFrameHeight = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_FRAME_HEIGHT).Content());
        cap.set(CAP_PROP_FRAME_HEIGHT, (double) realFrameHeight);
        CameraSetting frameHeight = CameraSetting("Height: ", CAP_PROP_FRAME_HEIGHT, 100, 2000, realFrameHeight);
        this->settings.push_back(frameHeight);
            editor.Pack_start(frameHeight.GetWidget(), true, true, 0);

        Label resolutionNote = Label(
            "NOTE: Some resolutions may not be supported by your camera!"
        );
            editor.Pack_start(resolutionNote.GetWidget(), true, true, 0);

        Separator hsep = Separator(true);
            editor.Pack_start(hsep.GetWidget(), true, true, 0);

        Label settingsHeader = Label("Other Settings");
            editor.Pack_start(settingsHeader.GetWidget(), true, true, 0);

        //exposure auto
        int realExposureAuto = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_AUTO_EXPOSURE).Content());
        CameraSetting autoExposure = CameraSetting("Auto Exposure: ", CAP_PROP_AUTO_EXPOSURE, 0, 6, realExposureAuto);
        this->settings.push_back(autoExposure);
            editor.Pack_start(autoExposure.GetWidget(), true, true, 0);

        //exposure
        int realExposure = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_EXPOSURE).Content());
        CameraSetting exposure = CameraSetting("Exposure: ", CAP_PROP_EXPOSURE, 0, 2000, realExposure);
        this->settings.push_back(exposure);
            editor.Pack_start(exposure.GetWidget(), true, true, 0);

        //white balance auto
        int realWhiteBalanceAuto = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_AUTO_WB).Content());
        CameraSetting autoWB = CameraSetting("Auto White Balance: ", CAP_PROP_AUTO_WB, 0, 6, 1);
        this->settings.push_back(autoWB);
            editor.Pack_start(autoWB.GetWidget(), true, true, 0);

        //white balance
        int realWhiteBalance = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_WB_TEMPERATURE).Content());
        CameraSetting WB = CameraSetting("White Balance: ", CAP_PROP_WB_TEMPERATURE, 0, 2000, 512);
        this->settings.push_back(WB);
            editor.Pack_start(WB.GetWidget(), true, true, 0);

        //auto focus
        int realAutoFocus = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_AUTOFOCUS).Content());
        CameraSetting autoFocus = CameraSetting("Auto Focus: ", CAP_PROP_AUTOFOCUS, 0, 6, 1);
        this->settings.push_back(autoFocus);
            editor.Pack_start(autoFocus.GetWidget(), true, true, 0);

        //focus
        int realFocus = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_FOCUS).Content());
        CameraSetting focus = CameraSetting("Focus: ", CAP_PROP_AUTOFOCUS, 0, 2000, 512);
        this->settings.push_back(focus);
            editor.Pack_start(focus.GetWidget(), true, true, 0);

        //brightness
        int realBrightness = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_BRIGHTNESS).Content());
        CameraSetting brightness = CameraSetting("Brightness: ", CAP_PROP_BRIGHTNESS, 0, 2000, 512);
        this->settings.push_back(brightness);
            editor.Pack_start(brightness.GetWidget(), true, true, 0);

        //gain
        int realGain = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_GAIN).Content());
        CameraSetting gain = CameraSetting("Gain: ", CAP_PROP_GAIN, 0, 2000, 512);
        this->settings.push_back(gain);
            editor.Pack_start(gain.GetWidget(), true, true, 0);

        //gamma
        int realGamma = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_GAMMA).Content());
        CameraSetting gamma = CameraSetting("Gamma: ", CAP_PROP_GAMMA, 0, 2000, 512);
        this->settings.push_back(gamma);
            editor.Pack_start(gamma.GetWidget(), true, true, 0);

        //the apply button
        Button apply = Button("Apply", KiwiLightApp::EditorApplyCameraSettings);
            editor.Pack_start(apply.GetWidget(), true, true, 0);

    this->settingsWidget = editor.GetWidget();
}

void Settings::Update() {

}


/**
 * Returns an XMLTag containing information describing the settings' values.
 */
XMLTag Settings::GetFinishedTag() {
    
}

/**
 * Finds the setting with the given ID and sets its value.
 */
void Settings::SetSettingValueFromID(int id, double value) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].GetValueName() == id) {
            this->settings[i].SetValue((int) value);
        }
    }
}


/**
 * Finds the setting with the given ID and returns it's value, or -1.0 if none found.
 */
double Settings::GetSettingValueFromID(int id) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].GetValueName() == id) {
            return this->settings[i].GetValue();
        }
    }

    return -1;
}


std::vector<int> Settings::GetSettingIDs() {
    std::vector<int> ids;
    for(int i=0; i<this->settings.size(); i++) {
        ids.push_back(this->settings[i].GetValueName());
    }

    return ids;
}


void Settings::SetName(std::string name) {
    gtk_widget_set_name(this->settingsWidget, name.c_str());
}