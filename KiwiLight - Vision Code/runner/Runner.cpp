#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


Runner::Runner(std::string fileName, bool debugging) {
    XMLDocument file = XMLDocument(fileName);
    parseDocument(file);
    
}


void Runner::SetSetting(std::string settingName, double value) {

}


double Runner::GetSetting(std::string settingName) {

}


void Runner::parseDocument(XMLDocument doc) {

}