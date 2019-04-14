#include "Runner.h"

/**
 * Source file for the ConfigurationSetting class.
 * Written By: Brach Knutson.
 */

using namespace KiwiLight;


ConfigurationSetting::ConfigurationSetting(std::string name, double value) {
    this->name = name;
    this->value = value;
}


void ConfigurationSetting::SetValue(double value) {
    this->value = value;
}