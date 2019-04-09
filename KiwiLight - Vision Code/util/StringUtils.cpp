#include "Util.h"

/**
 * Source file for the StringUtils class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Splits the given std::string str by the char character.
 */
std::vector<std::string> StringUtils::SplitString(std::string str, char character) {
    std::vector<std::string> substrings;
    std::string current = "";

    for(int i=0; i<str.length(); i++) {
        char currentCharacter = str.at(i);
        if(currentCharacter == character) {
            substrings.push_back(current);
            current = "";
        } else {
            current += currentCharacter;
        }
        
    }

    if(current.length() > 0) {
        substrings.push_back(current);
    }

    return substrings;
}