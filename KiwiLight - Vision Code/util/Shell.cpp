#include "Util.h"

/**
 * Source file for the Shell class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

std::string Shell::ExecuteCommand(std::string command) {
    std::string execute = command + " >> shell_out.log";
    system(execute.c_str());

    //create a file stream to view the log
    std::fstream log = std::fstream("shell_out.log");
    std::string logString = "";

    //loop through and read every line, formatting into std::string separated by newline characters
    std::string line;
    while(getline(log, line)) {
        logString += line + "\n";
    }

    log.close();

    //clear the file text for the next time
    std::ofstream file = std::ofstream("shell_out.log", std::ofstream::out | std::ofstream::trunc);
    file.close();

    return logString;
}