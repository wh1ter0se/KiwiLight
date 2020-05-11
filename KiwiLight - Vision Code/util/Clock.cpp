#include "Util.h"

/**
 * Source file for the Clock class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;
using namespace std::chrono;

Clock::Clock() {
}


void Clock::Start() {
    startTime = GetSystemTime();
}


long Clock::GetTime() {
    return GetSystemTime() - startTime;
}


long Clock::GetSystemTime() {
    milliseconds ms = duration_cast<milliseconds> (
		system_clock::now().time_since_epoch()
	);
	
	return ms.count();
}


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