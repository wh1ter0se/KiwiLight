#include "Util.h"

/**
 * Source file for the Flags class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

vector<string> Flags::flagNames = vector<string>();

/**
 * Raises a flag with the passed name.
 */
void Flags::RaiseFlag(string flagName) {
    if(!Flags::GetFlag(flagName)) { //flag not there, raise it
        Flags::flagNames.push_back(flagName);
    }
}

/**
 * Removes any flag with the given name.
 */
void Flags::LowerFlag(string flagName) {
    //repopulate the flag vector without the given flag
    vector<string> newFlags = vector<string>();
    for(int i=0; i<Flags::flagNames.size(); i++) {
        if(Flags::flagNames[i] != flagName) {
            newFlags.push_back(Flags::flagNames[i]);
        }
    }

    Flags::flagNames = newFlags;
}

/**
 * Raises or lowers the given flag based on the "raised" property.
 */
void Flags::SetFlagState(string flagName, bool raised) {
    if(Flags::GetFlag(flagName) && !raised) {
        Flags::LowerFlag(flagName); // if the flag is there and we want it lowered, lower it
    }
    else if(!Flags::GetFlag(flagName) && raised) {
        Flags::RaiseFlag(flagName); // if flag is not there and we want it raised, raise it
    }
}

/**
 * Searches for a raised flag with the given name. Returns true for raised, otherwise false.
 */
bool Flags::GetFlag(string flagName) {
    for(int i=0; i<flagNames.size(); i++) {
        if(Flags::flagNames[i] == flagName) {
            return true;
        }
    }

    return false;
}