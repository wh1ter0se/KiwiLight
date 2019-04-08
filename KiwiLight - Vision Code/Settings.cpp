#include "KiwiLight.h"

/**
 * Source file for the Settings class
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Create a new Settings menu.
 */
Settings::Settings() {
    //get the settings for the current camera, as well as their ranges.
    string settingsAsString = Shell::ExecuteCommand("v4l2-ctl --list-ctrls");
    vector<string> settingsList = StringUtils::SplitString(settingsAsString, '\n');

    Panel panel = Panel(false, 5);

    for(int i=0; i<settingsList.size(); i++) {
        //create a camera settings widget for each setting. 
        //this is done by parsing the input string for the name and values of the setting.

        string settingName   = StringUtils::SplitString(settingsList[i], ':')[0];
        string settingRanges = StringUtils::SplitString(settingsList[i], ':')[1];

        int min = searchAndReturnValue(settingRanges, "min");
        int max = searchAndReturnValue(settingRanges, "max");
        int value = searchAndReturnValue(settingRanges, "value");

        CameraSetting newSetting = CameraSetting(settingName, min, max, value);
        this->settings.push_back(newSetting);

        panel.Pack_start(newSetting, true, false, 0);

        //add a separator so it doesn't look like trash
        Separator sep = Separator(true);
            panel.Pack_start(sep, true, false, 0);
    }

    Button applyButton = Button("Apply Settings", Settings::ScheduleApplySettings);
        panel.Pack_start(applyButton, true, false, 5);

    this->settingsWidget = panel.GetWidget();
}

/**
 * Tells the current camera manager to close the stream during the next iteration.
 * The camera manager will then schedule the settings to be applied.
 */
void Settings::ScheduleApplySettings() {
    cout << "schedule" << endl;
    cout.flush();
    Flags::RaiseFlag("GetReadyToApplySettings");
}

/**
 * Checks for updates that need to be applied using the flagging system.
 */
void Settings::Update() {

    //check for the update settings flag. if raised, the camera stream is closed and we can update settings
    if(Flags::GetFlag("ApplySettings")) {
        cout << "Applying settings" << endl;
        cout.flush();
        Flags::LowerFlag("ApplySettings");
    }
}

/**
 * Assuming searchString is a v4l string, searches searchString for the value of term
 * and returns that. Returns -1 if no term found in searchString. 
 * ex. a searchString of "min=4" returns 4 when term is "min"
 */
int Settings::searchAndReturnValue(string searchString, string term) {
    vector<string> possibleMatches = StringUtils::SplitString(searchString, ' ');
    string searchTerm = term + "=";

    //go through the possibilities and find the term
    for(int i=0; i<possibleMatches.size(); i++) {
        
        int termPos = possibleMatches[i].find(searchTerm.c_str());
        if(termPos > -1) {
            //we found it, determine the value, convert to int and return
            string valueString = possibleMatches[i].substr(term.length() + 1);
            return std::stoi(valueString);
        }
    }

    return -1;
}