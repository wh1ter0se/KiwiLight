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

/**
 * Returns a substring of str.
 * @param str The string to create a substring out of.
 * @param begin The first character that the substring should have.
 * @param end The first character that the substring should NOT have.
 */
std::string StringUtils::Substring(std::string str, int begin, int end) {
    std::string substr = "";

    if(begin == end ||
       begin > end ||
       begin > str.length() ||
       end > str.length()) {
        return "";
    }

    for(int i=begin; i<end; i++) {
        substr += str.at(i);
    }

    return substr;
}

/**
 * Returns whether or not str starts with startsWith.
 * @param str The string to test.
 * @param startsWith The string that str is being tested for.
 */
bool StringUtils::StringStartsWith(std::string str, std::string startsWith) {
    std::cout << "ssw1" << std::endl;
    bool retval = ( StringUtils::Substring(str, 0, startsWith.length()) == startsWith );
    std::cout << "ssw2" << std::endl;
    return retval;
}

/**
 * Returns the number of times "character" appears in "str."
 */
int StringUtils::CountCharacters(std::string str, char character) {
    int counter = 0;

    for(int i=0; i<str.length(); i++) {
        if(str.at(i) == character) {
            counter++;
        }
    }

    return counter;
}