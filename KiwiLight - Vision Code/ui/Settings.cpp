#include "../KiwiLight.h"

/**
 * Source file for the Settings class
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Create a new Camera Settings menu.
 */
Settings::Settings(XMLDocument doc) {

    std::vector<XMLTag> docSettings = 
        doc.GetTagsByName("camera")[0]
        .GetTagsByName("settings")[0]
        .GetTagsByName("setting");

    Panel editor = Panel(false, 0);

        //camera index
        Panel cameraIndexPanel = Panel(true, 0);
            Label cameraIndexHeader = Label("Camera Index: ");
                cameraIndexPanel.Pack_start(cameraIndexHeader.GetWidget(), true, true, 0);

            int realCameraIndex = std::stoi(doc.GetTagsByName("camera")[0].GetAttributesByName("index")[0].Value());
            this->cameraIndex = NumberBox(0.0, 10.0, 1.0, realCameraIndex);
                cameraIndexPanel.Pack_start(this->cameraIndex.GetWidget(), true, true, 0);
            editor.Pack_start(cameraIndexPanel.GetWidget(), true, true, 0);

        //resolution area
        Label resolutionHeader = Label("Resolution");
            resolutionHeader.SetName("subHeader");
            editor.Pack_start(resolutionHeader.GetWidget(), true, true, 0);

        //frame width
        int realFrameWidth = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_FRAME_WIDTH).Content());
        CameraSetting frameWidth = CameraSetting("Width", CAP_PROP_FRAME_WIDTH, 100, 2000, realFrameWidth);
        this->settings.push_back(frameWidth);
            editor.Pack_start(frameWidth.GetWidget(), true, true, 0);

        //frame height
        int realFrameHeight = std::stoi(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_FRAME_HEIGHT).Content());
        CameraSetting frameHeight = CameraSetting("Height", CAP_PROP_FRAME_HEIGHT, 100, 2000, realFrameHeight);
        this->settings.push_back(frameHeight);
            editor.Pack_start(frameHeight.GetWidget(), true, true, 0);

        Label resolutionNote = Label(
            "NOTE: Some resolutions may not be supported by your camera!"
        );
            editor.Pack_start(resolutionNote.GetWidget(), true, true, 0);

        Separator hsep = Separator(true);
            editor.Pack_start(hsep.GetWidget(), true, true, 0);

        Label settingsHeader = Label("Other Settings");
            settingsHeader.SetName("subHeader");
            editor.Pack_start(settingsHeader.GetWidget(), true, true, 0);

        //exposure auto
        double realExposureAuto = std::stod(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_AUTO_EXPOSURE).Content());
        CameraSetting autoExposure = CameraSetting("Auto Exposure", CAP_PROP_AUTO_EXPOSURE, 0, 1, realExposureAuto);
        this->settings.push_back(autoExposure);
            editor.Pack_start(autoExposure.GetWidget(), true, true, 0);

        //exposure
        double realExposure = std::stod(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_EXPOSURE).Content());
        CameraSetting exposure = CameraSetting("Exposure", CAP_PROP_EXPOSURE, 0, 1, realExposure);
        this->settings.push_back(exposure);
            editor.Pack_start(exposure.GetWidget(), true, true, 0);

        //white balance auto
        double realWhiteBalanceAuto = std::stod(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_AUTO_WB).Content());
        CameraSetting autoWB = CameraSetting("Auto White Balance", CAP_PROP_AUTO_WB, 0, 1, realWhiteBalanceAuto);
        this->settings.push_back(autoWB);
            editor.Pack_start(autoWB.GetWidget(), true, true, 0);

        //white balance
        double realWhiteBalance = std::stod(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_WB_TEMPERATURE).Content());
        CameraSetting WB = CameraSetting("White Balance", CAP_PROP_WB_TEMPERATURE, -1, 1, realWhiteBalance);
        this->settings.push_back(WB);
            editor.Pack_start(WB.GetWidget(), true, true, 0);

        //brightness
        double realBrightness = std::stod(Util::SearchCameraSettingsByID(docSettings, CAP_PROP_BRIGHTNESS).Content());
        CameraSetting brightness = CameraSetting("Brightness", CAP_PROP_BRIGHTNESS, -1, 1, realBrightness);
        this->settings.push_back(brightness);
            editor.Pack_start(brightness.GetWidget(), true, true, 0);

        Panel buttonPanel = Panel(true, 0);
            Button frc = Button("FRC", KiwiLightApp::EditorApplyFRCSettings);
                buttonPanel.Pack_start(frc.GetWidget(), true, true, 0);

            //the apply chosen settings button
            Button apply = Button("Apply", KiwiLightApp::EditorApplyCameraSettings);
                buttonPanel.Pack_start(apply.GetWidget(), true, true, 0);

            editor.Pack_start(buttonPanel.GetWidget(), true, true, 0);

    this->widget = editor.GetWidget();
}

/**
 * Returns an XMLTag containing information describing the settings' values.
 */
XMLTag Settings::GetFinishedTag() {
    XMLTag settingsTag = XMLTag("settings");
        for(int i=0; i<this->settings.size(); i++) {
            XMLTag newSetting = XMLTag("setting", std::to_string(this->settings[i].GetValue()));
            //add the name attribute
                XMLTagAttribute settingAttr = XMLTagAttribute("id", std::to_string(this->settings[i].GetValueName()));
                newSetting.AddAttribute(settingAttr);

            settingsTag.AddTag(newSetting);
        }

    return settingsTag;
}

/**
 * Finds the setting with the given ID and sets its value.
 */
void Settings::SetSettingValueFromID(int id, double value) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].GetValueName() == id) {
            this->settings[i].SetValue(value);
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

/**
 * Sets the index of the camera being used.
 */
void Settings::SetCameraIndex(int index) {
    this->cameraIndex.SetValue((double) index);
}

/**
 * Return the index of the camera being used.
 */
int Settings::GetCameraIndex() {
    return this->cameraIndex.GetValue();
}

/**
 * Applies Recommended FRC camera settings.
 */
void Settings::ApplyFRCSettings() {
    SetSettingValueFromID(CAP_PROP_AUTO_EXPOSURE, 0);
    SetSettingValueFromID(CAP_PROP_EXPOSURE, 0);
    SetSettingValueFromID(CAP_PROP_AUTO_WB, 0);
    SetSettingValueFromID(CAP_PROP_WB_TEMPERATURE, 0);
    SetSettingValueFromID(CAP_PROP_BRIGHTNESS, 0);
}

/**
 * Returns the IDs of the settings being used.
 */
std::vector<int> Settings::GetSettingIDs() {
    std::vector<int> ids;
    for(int i=0; i<this->settings.size(); i++) {
        ids.push_back(this->settings[i].GetValueName());
    }

    return ids;
}
