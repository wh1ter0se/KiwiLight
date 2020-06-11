#include "Util.h"

/**
 * Source file for the SettingPair class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new SettingPair.
 * @param value The real value.
 * @param error The allowable error.
 */
SettingPair::SettingPair(double value, double error) {
    this->value = value;
    this->error = error;
}

/**
 * Returns the lower bound of the SettingPair's range.
 */
double SettingPair::LowerBound() {
    return this->value - this->error;
}

/**
 * Returns the upper bound of the SettingPair's range.
 */
double SettingPair::UpperBound() {
    return this->value + this->error;
}