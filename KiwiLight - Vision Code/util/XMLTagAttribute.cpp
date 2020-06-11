#include "Util.h"

/**
 * Source file for the XMLTagAttribute class.
 */

using namespace KiwiLight;

/**
 * Creates a new XMLTagAttribute with the given name and value.
 */
XMLTagAttribute::XMLTagAttribute(std::string name, std::string value) {
    this->name = name;
    this->value = value;
}

/**
 * Returns a string representing this XMLTagAttribute.
 */
std::string XMLTagAttribute::ReturnString() {
    return name + "=\"" + value + "\"";
}