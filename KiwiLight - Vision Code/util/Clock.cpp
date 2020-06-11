#include "Util.h"

/**
 * Source file for the Clock class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;
using namespace std::chrono;

/**
 * Creates a new Clock.
 */
Clock::Clock() {
}

/**
 * Marks the start time of the clock as now.
 */
void Clock::Start() {
    startTime = GetSystemTime();
}

/**
 * Returns the number of milliseconds that have elapsed since Start() was called.
 */
long Clock::GetTime() {
    return GetSystemTime() - startTime;
}

/**
 * Returns the time since epoch according to the system.
 */
long Clock::GetSystemTime() {
    milliseconds ms = duration_cast<milliseconds> (
		system_clock::now().time_since_epoch()
	);
	
	return ms.count();
}

/**
 * Returns the current date.
 * Format: MM-DD-YYYY-HH-MM-SS
 */
std::string Clock::GetDateString() {
    //get system time in seconds
    time_t currentTime;
    time(&currentTime);

    //pull date information
    struct tm *timeinfo = localtime(&currentTime);

    //convert to string
    char stringBuffer[100];
    strftime(stringBuffer, sizeof(stringBuffer), "%m-%d-%Y-%H-%M-%S", timeinfo);

    return std::string(stringBuffer);
}