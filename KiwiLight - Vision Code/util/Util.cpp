#include "Util.h"

/**
 * Source file for the Util class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

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
