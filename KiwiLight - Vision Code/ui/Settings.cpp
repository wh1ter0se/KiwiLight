#include "UI.h"

/**
 * Source file for the Settings class
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Create a new Settings menu.
 */
Settings::Settings(int index) {
    //get the settings for the current camera, as well as their ranges.
    std::string command = "v4l2-ctl -d " + std::to_string(index) + " --list-ctrls";
    std::string settingsAsString = Shell::ExecuteCommand(command);
    std::vector<std::string> settingsList = StringUtils::SplitString(settingsAsString, '\n');

    Panel panel = Panel(false, 5);

    this->frameWidth  = CameraSetting("Frame Width (int)", 1, 2000, 600);
        panel.Pack_start(frameWidth.GetWidget(), true, false, 0);

    this->frameHeight = CameraSetting("Frame Height (int)", 1, 2000, 400);
        panel.Pack_start(frameHeight.GetWidget(), true, false, 0);

    Label warning = Label("NOTE: Some resolutions may not be supported by your camera!");
        panel.Pack_start(warning.GetWidget(), true, false, 0);

    Separator sep = Separator(true);
        panel.Pack_start(sep.GetWidget(), true, false, 0);

    for(int i=0; i<settingsList.size(); i++) {
        //create a camera settings widget for each setting. 
        //this is done by parsing the input std::string for the name and values of the setting.

        std::string settingName   = StringUtils::SplitString(settingsList[i], ':')[0];
        std::string settingRanges = StringUtils::SplitString(settingsList[i], ':')[1];

        int min = searchAndReturnValue(settingRanges, "min");
        int max = searchAndReturnValue(settingRanges, "max");
        int value = searchAndReturnValue(settingRanges, "value");

        this->settings.push_back(CameraSetting(settingName, min, max, value));
        panel.Pack_start(this->settings[i].GetWidget(), true, false, 0);
    }

    Panel buttonPanel = Panel(true, 0);
        Button applyButton = Button("Apply Settings", Settings::ScheduleApplySettings);
            buttonPanel.Pack_start(applyButton.GetWidget(), true, true, 5);

        panel.Pack_start(buttonPanel.GetWidget(), true, false, 0);

    this->settingsWidget = panel.GetWidget();
}

/**
 * Tells the current camera manager to close the stream during the next iteration.
 * The camera manager will then schedule the settings to be applied.
 */
void Settings::ScheduleApplySettings() {
    Flags::RaiseFlag("CloseCamera");
}

/**
 * Checks for updates that need to be applied using the flagging system.
 */
void Settings::Update() {

    //check for the update settings flag. if raised, the camera stream is closed and we can update settings
    if(Flags::GetFlag("CameraClosed")) {
        Flags::LowerFlag("CameraClosed");

        this->camWidth  = this->frameWidth.GetValue();
        this->camHeight = this->frameHeight.GetValue();

        //go through all settings, grab values, use Shell to set them
        for(int i=0; i<this->settings.size(); i++) {

            std::string setCommand = "v4l2-ctl --set-ctrl=" + this->settings[i].GetName() + "=" +
                                                      std::to_string(this->settings[i].GetValue());

            Shell::ExecuteCommand(setCommand);

            // to improve ease of access, get the value of the setting and update the widget.
            std::string getCommand = "v4l2-ctl --get-ctrl=" + this->settings[i].GetName();
            std::string newValue = Shell::ExecuteCommand(getCommand);

            // when run, getCommand will return a string in this format:
            // [setting name]:[value]

            int valueInt = std::stoi(StringUtils::SplitString(newValue, ':')[1]);
            this->settings[i].SetValue(valueInt);
        }

        Flags::RaiseFlag("StartCamera");
    }
}


XMLTag Settings::GetFinishedTag() {
    XMLTag finishedTag = XMLTag("settings");
    
    for(int i=0; i<this->settings.size(); i++) {
        std::string settingName = this->settings[i].GetName();
        std::string settingValue = std::to_string(this->settings[i].GetValue());

        XMLTagAttribute newTagNameAttr = XMLTagAttribute("name", settingName);
        std::vector<XMLTagAttribute> attrList = std::vector<XMLTagAttribute>();
        attrList.push_back(newTagNameAttr);

        XMLTag newTag = XMLTag("setting", settingValue, attrList);
        finishedTag.AddTag(newTag);
    }

    return finishedTag;
}

/**
 * Assuming searchString is a v4l std::string, searches searchString for the value of term
 * and returns that. Returns -1 if no term found in searchString. 
 * ex. a searchString of "min=4" returns 4 when term is "min"
 */
int Settings::searchAndReturnValue(std::string searchString, std::string term) {
    std::vector<std::string> possibleMatches = StringUtils::SplitString(searchString, ' ');
    std::string searchTerm = term + "=";

    //go through the possibilities and find the term
    for(int i=0; i<possibleMatches.size(); i++) {
        
        int termPos = possibleMatches[i].find(searchTerm.c_str());
        if(termPos > -1) {
            //we found it, determine the value, convert to int and return
            std::string valueString = possibleMatches[i].substr(term.length() + 1);
            return std::stoi(valueString);
        }
    }

    return -1;
}


void Settings::SetName(std::string name) {
    gtk_widget_set_name(this->settingsWidget, name.c_str());
}