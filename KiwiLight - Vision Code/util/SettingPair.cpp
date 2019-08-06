#include "Util.h"

/**
 * Source file for the SettingPair class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


SettingPair::SettingPair(double value, double error) {
    this->value = value;
    this->error = error;
}


double SettingPair::LowerBound() {
    return this->value - this->error;
}


double SettingPair::UpperBound() {
    return this->value + this->error;
}