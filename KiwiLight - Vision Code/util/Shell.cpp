#include "Util.h"

/**
 * Source file for the Shell class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Executes "command" in the terminal logs its output, and returns it.
 * @param command The command to execute in the terminal.
 * @return The output of the command.
 */
std::string Shell::ExecuteCommand(std::string command) {
    //to create the out file for the command, find HOME to place it in KiwiLightData
    std::string outLocation = "";
    char *home = getenv("HOME");
    if(home != NULL) {
        outLocation = std::string(home) + "/KiwiLightData/tmp/shell_out.log";
    } else {
        std::cout << "The Shell utility was unable to find HOME" << std::endl;
    }

    std::string execute = command + " >> " + outLocation;
    system(execute.c_str());

    //create a file stream to view the log
    std::fstream log = std::fstream(outLocation);
    std::string logString = "";

    //loop through and read every line, formatting into std::string separated by newline characters
    std::string line;
    while(getline(log, line)) {
        logString += line + "\n";
    }

    log.close();

    //clear the file text for the next time
    std::ofstream file = std::ofstream(outLocation, std::ofstream::out | std::ofstream::trunc);
    file.close();

    return logString;
}