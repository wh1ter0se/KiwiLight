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
    h = (h * -1) + 360;
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


/**
 * Sorts "data" in order from least to greatest. Sorts using insertion sort.
 */
std::vector<double> Util::SortLeastGreatestDouble(std::vector<double> data) {
    std::vector<double> result;

    for(int i=0; i<data.size(); i++) {
        double valueToInsert = data[i];

        // if there is nothing in result or valueToInsert is less than the smallest value...
        if(result.size() == 0 || result[0] >= valueToInsert) {
            result.emplace(result.begin(), valueToInsert);
        } else {
            //look for a position to insert the value in
            bool placeToInsertFound = false;
            for(int k=1; k<result.size(); k++) {
                double valueBeforeI = result[k - 1];
                double valueAtI = result[k];

                if(valueToInsert > valueBeforeI && valueToInsert <= valueAtI) {
                    result.emplace(result.begin() + k, valueToInsert);
                    placeToInsertFound = true;
                    break;
                }
            }

            //value is greater than everything in the list
            if(!placeToInsertFound) {
                result.emplace(result.end(), valueToInsert);
            }
        }
    }

    return result;
}

/**
 * Returns the greatest value of the set after outliers are removed.
 */
double Util::MaxWithoutOutliers(std::vector<double> data, double allowableDeviation) {
    std::vector<double> noOutliers = Util::RemoveOutliers(data, allowableDeviation);
    return Util::Greatest(noOutliers);
}

/**
 * Returns the sum of all numbers in the set.
 */
double Util::Total(std::vector<double> data) {
    double total = 0.0;

    for(int i=0; i<data.size(); i++) {
        total += data[i];
    }

    return total;
}

/**
 * Returns the average of all numbers in the set.
 */
double Util::Average(std::vector<double> data) {
    double avg = 0.0;

    for(int i=0; i<data.size(); i++) {
        avg += data[i];
    }

    avg /= data.size();
    return avg;
}

/**
 * Returns the median of all numbers in the set.
 */
double Util::Median(std::vector<double> data) {
    int middle = data.size() / 2;
    return data[middle];
}

/**
 * Returns the greatest value in the set.
 */
double Util::Greatest(std::vector<double> data) {
    double greatest = -1000000000.0;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        if(value > greatest) {
            greatest = value;
        }
    }

    return greatest;
}

/**
 * Returns the smallest value in the set.
 */
double Util::Least(std::vector<double> data) {
    double least = 1000000000.0;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        if(value < least) {
            least = value;
        }
    }

    return least;
}

/**
 * Returns the average difference of all numbers in the set.
 */
double Util::AverageDifference(std::vector<double> data) {
    double avgDiff = 0.0;
    int totalDifferences = 0;

    for(int i=0; i<data.size() - 1; i++) {
        double val1 = data[i];
        double val2 = data[i + 1];
        double difference = abs(val1 - val2);
        avgDiff += difference;

        totalDifferences++;
    }

    avgDiff /= (double) totalDifferences;
    return avgDiff;
}

/**
 * Returns the number of times "value" occurs in the set.
 */
int Util::NumberOfOccurrances(std::vector<double> data, double value) {
    int occurrances = 0;

    for(int i=0; i<data.size(); i++) {
        if(data[i] == value) {
            occurrances++;
        }
    }

    return occurrances;
}

/**
 * Returns the value with the most occurances in the set.
 */
double Util::MostCommonValue(std::vector<double> data) {
    int indexWithMostOccurrances = 0;
    int numberOfOccurrances = -1;

    for(int i=0; i<data.size(); i++) {
        double value = data[i];
        int occurrances = Util::NumberOfOccurrances(data, value);
        if(occurrances > numberOfOccurrances) {
            numberOfOccurrances = occurrances;
            indexWithMostOccurrances = i;
        }
    }

    return data[indexWithMostOccurrances];
}

/**
 * Returns whether or not the value at "indexOfValue" is an outlier in the set "data"
 */
bool Util::IsOutlier(std::vector<double> data, int indexOfValue, double allowableError) {
    double average = Util::Average(data);
    double upperBound = average + allowableError;
    double lowerBound = average - allowableError;

    return !(data[indexOfValue] > lowerBound && data[indexOfValue] < upperBound);
}

/**
 * Removes all outliers from the set.
 * @param data The data set.
 * @param allowableError The allowable devation from the median to not be considered an outlier.
 */
std::vector<double> Util::RemoveOutliers(std::vector<double> data, double allowableError) {
    std::vector<double> newSet = std::vector<double>();
    std::vector<double> sorted = Util::SortLeastGreatestDouble(data);
    double dataAvg = Util::Median(data);

    for(int i=0; i<data.size(); i++) {
        if(abs(data[i] - dataAvg) <= allowableError) {
            newSet.push_back(data[i]);
        }
    }

    return newSet;
}

/**
 * Removes all occurances of "occurances" from data.
 */
std::vector<double> Util::RemoveOccurances(std::vector<double> data, double occurance) {
    std::vector<double> newSet = std::vector<double>();

    for(int i=0; i<data.size(); i++) {
        if(data[i] != occurance) {
            newSet.push_back(data[i]);
        }
    }

    return newSet;
}

/**
 * Casts all doubles in "data" to int and returns the vector.
 */
std::vector<int> Util::VectorDoubleToInt(std::vector<double> data) {
    std::vector<int> result;
    for(int i=0; i<data.size(); i++) {
        result.push_back((int) data[i]);
    }

    return result;
}

/**
 * Cast all ints in "data" to double.
 */
std::vector<double> Util::VectorIntToDouble(std::vector<int> data) {
    std::vector<double> result;
    for(int i=0; i<data.size(); i++) {
        result.push_back((double) data[i]);
    }

    return result;
}

/**
 * Encodes data into a string.
 */
std::string Util::VectorToString(std::vector<double> data) {
    std::string vectorString = "[";

    for(int i=0; i<data.size(); i++) {
        vectorString += " " + std::to_string(data[i]);

        if(i < data.size() - 1) {
            vectorString += ", ";
        }
    }

    vectorString += " ]";
    return vectorString;
}

/**
 * Splits the given std::string str by the char character.
 */
std::vector<std::string> Util::SplitString(std::string str, char character) {
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
std::string Util::Substring(std::string str, int begin, int end) {
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
 * Returns whether or not str starts with sequence.
 * @param str The string to test.
 * @param sequence The string that str is being tested for.
 */
bool Util::StringStartsWith(std::string str, std::string sequence) {
    return Util::Substring(str, 0, sequence.length()) == sequence;
}

/**
 * Returns whether or not str ends with sequence.
 */
bool Util::StringEndsWith(std::string str, std::string sequence) {
    return Util::Substring(str, str.length() - sequence.length(), str.length()) == sequence;
}

/**
 * Returns the number of times "character" appears in "str."
 */
int Util::CountCharacters(std::string str, char character) {
    int counter = 0;

    for(int i=0; i<str.length(); i++) {
        if(str.at(i) == character) {
            counter++;
        }
    }

    return counter;
}