#include "Util.h"

/**
 * Source file for the StringUtils class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Splits the given string str by the char character.
 */
vector<string> StringUtils::SplitString(string str, char character) {
    vector<string> substrings;
    string current = "";

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