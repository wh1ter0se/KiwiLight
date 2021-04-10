#include "Util.h"

/**
 * Source file for the Util class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Returns the file path to KiwiLight's generic.xml, or "" if none was found.
 */
std::string Util::ResolveGenericConfFilePath() {
    std::string pathToOpen = "";
    char *homedir = getenv("HOME");
    if(homedir == NULL) {
        std::cout << "The HOME environment variable could not be found." << std::endl;
    } else {
        pathToOpen = std::string(homedir) + "/KiwiLightData/confs/generic.xml";
    }

    return pathToOpen;
}

/**
 * Searches the vector of camera setting XMLTags and returns the one with the correct ID.
 * @param settings The list of camera settings to search through. See precondition for more details.
 * @param id The Camera property ID to search for.
 * @return The first XMLTag with the matching ID, or NULL if none found.
 * Precondition: The XMLTags in settings MUST be formatted as follows:
 * <tag id="[integer]">[content]</tag>
 */
XMLTag Util::SearchCameraSettingsByID(std::vector<XMLTag> settings, int id) {
    for(int i=0; i<settings.size(); i++) {
        XMLTag setting = settings[i];
        int settingID = std::stoi(setting.GetAttributesByName("id")[0].Value());
        if(settingID == id) {
            return setting;
        }
    }

    std::cout << "No Camera Setting could be found by ID " << id << "!" << std::endl;
    return settings[0];
}

/**
 * Evaluates the validity of a KiwiLight config file.
 * Will check for the following tags to decide if the file is valid:
 * <camera></camera>
 * <configuration></configuration>
 * <preprocessor></preprocessor>
 * <postprocessor>
 *      <contour></contour>
 *      <UDP></UDP>
 * </postprocessor>
 * 
 * @param filename The path to the config file to check
 * @return {@code true} if the file is valid, {@code false} otherwise.
 */
bool Util::configDocumentIsValid(XMLDocument document) {
    XMLTag
        camera        = document.GetTagsByName("camera")[0],
        configuration = document.GetTagsByName("configuration")[0],
        preprocessor  = configuration.GetTagsByName("preprocessor")[0],
        postprocessor = configuration.GetTagsByName("postprocessor")[0],
        contour       = postprocessor.GetTagsByName("contour")[0],
        udp           = postprocessor.GetTagsByName("UDP")[0];

    //check for the contents of any of those tags to be "NULL". If such is true, then the document is not valid.
    bool valid = true;
    valid = valid && camera.Content()        != "NULL";
    valid = valid && configuration.Content() != "NULL";
    valid = valid && preprocessor.Content()  != "NULL";
    valid = valid && postprocessor.Content() != "NULL";
    valid = valid && contour.Content()       != "NULL";
    valid = valid && udp.Content()           != "NULL";

    return valid;
}