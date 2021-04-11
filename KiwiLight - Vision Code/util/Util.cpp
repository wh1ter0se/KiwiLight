#include "Util.h"

/**
 * Source file for the Util class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

const char *Util::HEX_DIGITS = "0123456789abcdef";

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
 * Creates a formatted string that can be sent to the receiver on the RIO.
 * @param id The ID of the target.
 * @param x The x-coordintae of the target (px)
 * @param y The y-coordinate of the target (px).
 * @param width The width of the target (px).
 * @param height The height of the target (px).
 * @param distance The distance to the target.
 * @param horizontalAngle The angle to the target, horizontally (degrees)
 * @param verticalAngle The angle to the target, vertically (degrees)
 * @return Formatted message for the RIO.
 */
std::string Util::composeRioMessage (
    int id,
    int x,
    int y,
    int width,
    int height,
    double distance,
    double horizontalAngle,
    double verticalAngle
) {
    std::string 
        ids = std::to_string(id),
        xs = std::to_string(x),
        ys = std::to_string(y),
        w = std::to_string(width),
        h = std::to_string(height),
        d = std::to_string(Util::roundTo(distance, 2)),
        ax = std::to_string(Util::roundTo(horizontalAngle, 2)),
        ay = std::to_string(Util::roundTo(verticalAngle, 2));

    //shorten the stringed doubles so that they look nice
    d = shortenStringedDouble(d, 2);
    ax = shortenStringedDouble(ax, 2);
    ay = shortenStringedDouble(ay, 2);

    return ":" + ids + "," + xs + "," + ys + "," + w + "," + h +"," + d + "," + ax + "," + ay + ";";
}

/**
 * Shortens a string created by std::to_string() so that it only shows a certain number of decimal places.
 * @param str The string to shorten.
 * @param places The number of decimal places to show.
 * @return The shortened string.
 */
std::string Util::shortenStringedDouble(std::string str, int places) {
    std::size_t dp = str.find('.');
    if(dp != std::string::npos) {
        return str.substr(0, dp + 3);
    }

    return str;
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
 *      <target>
 *          <contour></contour>
 *      </target>
 *      <UDP></UDP>
 * </postprocessor>
 * 
 * @param document The config file to check
 * @return {@code true} if the file is valid, {@code false} otherwise.
 */
bool Util::configDocumentIsValid(XMLDocument document) {
    XMLTag
        camera        = document.GetTagsByName("camera")[0],
        configuration = document.GetTagsByName("configuration")[0],
        preprocessor  = configuration.GetTagsByName("preprocessor")[0],
        postprocessor = configuration.GetTagsByName("postprocessor")[0],
        target        = postprocessor.GetTagsByName("target")[0],
        contour       = target.GetTagsByName("contour")[0],
        udp           = postprocessor.GetTagsByName("UDP")[0];

    //check for the contents of any of those tags to be "NULL". If such is true, then the document is not valid.
    bool valid = true;
    valid = valid && camera.Content()        != "NULL";
    valid = valid && configuration.Content() != "NULL";
    valid = valid && preprocessor.Content()  != "NULL";
    valid = valid && postprocessor.Content() != "NULL";
    valid = valid && target.Content()        != "NULL";
    valid = valid && contour.Content()       != "NULL";
    valid = valid && udp.Content()           != "NULL";

    return valid;
}

/**
 * Evalutes the validity of a KiwiLight log file.
 * Will check for the following tags to decide:
 * <KiwiLightLog>
 *      <TotalFrames></TotalFrames>
 *      <FramesWithTargetSeen></FramesWithTargetSeen>
 *      <AverageFPS></AverageFPS>
 *      <AverageDistance></AverageDistance>
 *      <Events></Events>
 * </KiwiLightLog>
 */
bool Util::logDocumentIsValid(XMLDocument document) {
    XMLTag
        kiwiLightLog         = document.GetTagsByName("KiwiLightLog")[0],
        totalFrames          = kiwiLightLog.GetTagsByName("TotalFrames")[0],
        framesWithTargetSeen = kiwiLightLog.GetTagsByName("FramesWithTargetSeen")[0],
        averageFPS           = kiwiLightLog.GetTagsByName("AverageFPS")[0],
        averageDistance      = kiwiLightLog.GetTagsByName("AverageDistance")[0],
        events               = kiwiLightLog.GetTagsByName("Events")[0];

    //check for the contents of any tags to be "NULL" if such is true then the document is not valid
    bool valid = true;
    valid = valid && kiwiLightLog.Content()         != "NULL";
    valid = valid && totalFrames.Content()          != "NULL";
    valid = valid && framesWithTargetSeen.Content() != "NULL";
    valid = valid && averageFPS.Content()           != "NULL";
    valid = valid && averageDistance.Content()      != "NULL";
    valid = valid && events.Content()               != "NULL";

    return valid;
}

/**
 * Attempts to convert a string to double and returns a backup if it cant.
 * @param str The string to convert
 * @param backup The backup to return if an error occurs.
 * @return Double value.
 */
double Util::toDouble(std::string str, double backup) {
    try {
        return std::stod(str);
    } catch(std::invalid_argument ex) {
        return backup;
    }
}

/**
 * Attempts to convert a string to int and returns a backup if it cant.
 * @param str The string to convert
 * @param backup The backup to return if an error occurs.
 * @return Int value.
 */
int Util::toInt(std::string str, int backup) {
    try {
        return std::stoi(str);
    } catch(std::invalid_argument ex) {
        return backup;
    }
}

/**
 * Rounds a number to a certain number of places.
 * @param number The number to round.
 * @param places The number of places to round the number to.
 * @return The rounded number.
 */
double Util::roundTo(double number, int places) {
    double multiplier = pow(10, places);
    double oldNumber = number * multiplier;
    oldNumber += 0.5; //rounds the number up if greater than .5
    
    int roundedNumber = (int) oldNumber;

    return roundedNumber / multiplier;
}

/**
 * Converts a color in HSV to RGB. Uses the formula on RapidTables.
 * @param h Hue value from 0 to 360
 * @param s Saturation value from 0 to 100
 * @param v Value (brightness) value from 0 to 100
 * @return Color in RGB
 */
Scalar Util::toRGB(double h, double s, double v) {
    double c = (v / 100.0) * (s / 100.0);
    double x = c * (1 - abs(fmod(h / 60.0, 2.0) - 1));
    double m = (v / 100.0) - c;

    double
        r0 = 0,
        g0 = 0,
        b0 = 0;

    if(h >= 0 && h < 60) {
        r0 = c;
        g0 = x;
    } else if(h < 120) {
        r0 = x;
        g0 = c;
    } else if(h < 180) {
        g0 = c;
        b0 = x;
    } else if(h < 240) {
        g0 = x;
        b0 = c;
    } else if(h < 300) {
        r0 = x;
        b0 = c;
    } else if(h <= 360) {
        r0 = c;
        b0 = x;
    }

    int
        r = (r0 + m) * 255,
        g = (g0 + m) * 255,
        b = (b0 + m) * 255;

    return Scalar(r, g, b);
}

/**
 * Converts a value to hexadecimals and returns a string representing that hex value.
 * @param value The value to convert to hex.
 * @return The value in hexadecimal.
 */
std::string Util::hexify(int value) {
    int dividend = value;
    std::string digits = "";
    while(dividend > 0) {
        int remainder = dividend % 16;
        dividend /= 16;

        digits.insert(0, 1, HEX_DIGITS[remainder]);
    }

    if(digits.length() == 0) {
        digits = "00";
    }

    return digits;
}

/**
 * Converts an HSV value to a hex color.
 */
std::string Util::getHexColor(double h, double s, double v) {
    Scalar rgb = toRGB(h, s, v);

    //define array for hexed number values 
    std::string hexvals[3];

    //put number values into string values array
    for(int i=0; i<3; i++) {
        
        std::string hexval = hexify(rgb[i]);
        
        while(hexval.length() < 2) {
            hexval.insert(0, 1, '0');
        }

        if(hexval.length() > 2) {
            hexval =  "ff"; //cap hex value to max of ff (full value for 2 digits)
        }

        hexvals[i] = hexval;
    }
    std::string final = "#" + hexvals[0] + hexvals[1] + hexvals[2];
    return final;
}