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
    time_t currentTime;
    time(&currentTime);
    std::cout << "currentTime: " << currentTime << std::endl;
}