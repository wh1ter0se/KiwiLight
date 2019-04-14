#include "Runner.h"

/**
 * Source file for the ConfigurationSettingList
 */

using namespace KiwiLight;


ConfigurationSettingsList::ConfigurationSettingsList() {
}


ConfigurationSettingsList::ConfigurationSettingsList(XMLTag tag) {
    
}


void ConfigurationSettingsList::AddSetting(std::string settingName, double value) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName)
            return;
    }

    ConfigurationSetting newSetting = ConfigurationSetting(settingName, value);
    this->settings.push_back(newSetting);
}


void ConfigurationSettingsList::SetSetting(std::string settingName, double value) {
    //find the setting
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName) {
            this->settings[i].SetValue(value);
            return;
        }
    }
}


double ConfigurationSettingsList::GetSetting(std::string settingName) {
    for(int i=0; i<this->settings.size(); i++) {
        if(this->settings[i].Name() == settingName) {
            return this->settings[i].GetValue();
        }
    }
}