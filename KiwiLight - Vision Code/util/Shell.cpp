#include "Util.h"

/**
 * Source file for the Shell class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

string Shell::ExecuteCommand(string command) {
    string execute = command + " >> shell_out.log";
    system(execute.c_str());

    //create a file stream to view the log
    fstream log = fstream("shell_out.log");
    string logString = "";

    //loop through and read every line, formatting into string separated by newline characters
    string line;
    while(getline(log, line)) {
        logString += line + "\n";
    }

    log.close();

    //clear the file text for the next time
    ofstream file = ofstream("shell_out.log", std::ofstream::out | std::ofstream::trunc);
    file.close();

    return logString;
}