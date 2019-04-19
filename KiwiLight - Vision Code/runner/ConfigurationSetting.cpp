#include "Runner.h"

/**
 * Source file for the ConfigurationSetting class.
 * Written By: Brach Knutson.
 */

using namespace KiwiLight;


ConfigurationSetting::ConfigurationSetting(std::string name, std::string value) {
    this->name = name;
    this->value = value;
}


void ConfigurationSetting::SetValue(std::string value) {
    this->value = value;
}