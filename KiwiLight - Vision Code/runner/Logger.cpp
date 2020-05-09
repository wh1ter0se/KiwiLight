#include "Runner.h"

/**
 * Source file for the Logger class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

const int Logger::LOG_ARRAY_MAX_SIZE = 50;

Logger::Logger(std::string filePath) {
    this->filePath = filePath;
    fpsSamples = new int[LOG_ARRAY_MAX_SIZE];
}


void Logger::Start() {
    clock.Start();
}


void Logger::Log(std::string runnerOutput) {
    std::cout << "logger time: " << clock.GetTime();
}