#include "Runner.h"

/**
 * Source file for the ConfigurationSettingList
 */

using namespace KiwiLight;


ConfigurationSettingsList::ConfigurationSettingsList() {
}

void ConfigurationSettingsList::AddSetting(std::string settingName, std::string value) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName)
            return;
    }

    ConfigurationSetting newSetting = ConfigurationSetting(settingName, value);
    this->settings.push_back(newSetting);
}


void ConfigurationSettingsList::SetSetting(std::string settingName, std::string value) {
    //find the setting
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName) {
            this->settings[i].SetValue(value);
            return;
        }
    }
}


std::string ConfigurationSettingsList::GetSetting(std::string settingName) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName) {
            return this->settings[i].GetValue();
        }
    }

    return "";
}