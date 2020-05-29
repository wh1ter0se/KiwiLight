#include "Util.h"

/**
 * Source file for the Flags class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

std::vector<std::string> Flags::flagNames = std::vector<std::string>();

/**
 * Raises a flag with the passed name.
 * DEPRECATED: The Flags class will be removed in the next update.
 */
[[deprecated("The Flags class is no longer used and will be removed in the next update.")]]
void Flags::RaiseFlag(std::string flagName) {
    if(!Flags::GetFlag(flagName)) { //flag not there, raise it
        Flags::flagNames.push_back(flagName);
    }
}

/**
 * Removes any flag with the given name.
 * DEPRECATED: The Flags class will be removed in the next update.
 */
[[deprecated("The Flags class is no longer used and will be removed in the next update.")]]
void Flags::LowerFlag(std::string flagName) {
    //repopulate the flag std::vector without the given flag
    std::vector<std::string> newFlags = std::vector<std::string>();
    for(int i=0; i<Flags::flagNames.size(); i++) {
        if(Flags::flagNames[i] != flagName) {
            newFlags.push_back(Flags::flagNames[i]);
        }
    }

    Flags::flagNames = newFlags;
}

/**
 * Raises or lowers the given flag based on the "raised" property.
 * DEPRECATED: The Flags class will be removed in the next update.
 */
[[deprecated("The Flags class is no longer used and will be removed in the next update.")]]
void Flags::SetFlagState(std::string flagName, bool raised) {
    if(Flags::GetFlag(flagName) && !raised) {
        Flags::LowerFlag(flagName); // if the flag is there and we want it lowered, lower it
    }
    else if(!Flags::GetFlag(flagName) && raised) {
        Flags::RaiseFlag(flagName); // if flag is not there and we want it raised, raise it
    }
}

/**
 * Searches for a raised flag with the given name. Returns true for raised, otherwise false.
 * DEPRECATED: The Flags class will be removed in the next update.
 */
[[deprecated("The Flags class is no longer used and will be removed in the next update.")]]
bool Flags::GetFlag(std::string flagName) {
    for(int i=0; i<flagNames.size(); i++) {
        if(Flags::flagNames[i] == flagName) {
            return true;
        }
    }

    return false;
}