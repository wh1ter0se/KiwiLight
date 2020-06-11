#include "Util.h"

/**
 * Source file for the LogEvent class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

const std::string LogEvent::RECORD_LOW_FPS   = "RL_FPS";
const std::string LogEvent::RECORD_HIGH_FPS  = "RH_FPS";
const std::string LogEvent::RECORD_LOW_DIST  = "RL_DIST";
const std::string LogEvent::RECORD_HIGH_DIST = "RH_DIST";
const std::string LogEvent::GENERAL_UPDATE   = "UPDATE";

/**
 * Creates a new LogEvent. This constructor should only be used for the following events:
 * RECORD_LOW_FPS
 * RECORD_HIGH_FPS
 * RECORD_LOW_DIST
 * RECORD_HIGH_DIST
 */
LogEvent::LogEvent(const std::string evName, long timestamp, double record) {
    InitBlank(evName, timestamp);
    //assert that evName is correct
    std::string evNameString = std::string(evName);
    if(
        evName == std::string(RECORD_LOW_FPS) ||
        evName == std::string(RECORD_HIGH_FPS) ||
        evName == std::string(RECORD_LOW_DIST) ||
        evName == std::string(RECORD_HIGH_DIST)
    ) {
        this->record = record;
    } else {
        std::cout << "LogEvent ERROR: Wrong event type \"" << evName << "\" used in construcor (name, record)! LogEvent will be undefined!" << std::endl;
    }
}

/**
 * Creates a new LogEvent. This constructor should only be used for the following events:
 * GENERAL_UPDATE
 */
LogEvent::LogEvent(const std::string evName, long timestamp, double fps, int distance, bool targetSeen) {
    InitBlank(evName, timestamp);
    //assert that evName is correct
    if(std::string(evName) == std::string(GENERAL_UPDATE)) {
        this->fps = fps;
        this->distance = distance;
        this->targetSeen = targetSeen;
    } else {
        std::cout << "LogEvent ERROR: Wrong event type \"" << evName << "\" used in constructor (name, fps, distance, targetSeen)! LogEvent will be undefined!" << std::endl;
    }
}

/**
 * Returns the event type.
 */
std::string LogEvent::GetEventType() {
    return std::string(this->evName);
}

/**
 * Returns the event timestamp in milliseconds.
 */
long LogEvent::GetTimestamp() {
    return this->timestamp;
}

/**
 * Returns the record for this LogEvent, or -1 if there is none.
 */
double LogEvent::GetRecord() {
    return this->record;
}

/**
 * Returns the FPS for this LogEvent, or -1 if there is none.
 */
double LogEvent::GetFPS() {
    return this->fps;
}

/**
 * Returns the Distance for this LogEvent, or -1 if there is none.
 */
int LogEvent::GetDistance() {
    return this->distance;
}

/**
 * Returns whether or not the target was seen when this event was recorded.
 */
bool LogEvent::GetTargetSeen() {
    return this->targetSeen;
}

/**
 * Encodes this LogEvent into an XMLTag.
 */
XMLTag LogEvent::EncodeXMLTag() {
    XMLTag tag = XMLTag("Event");

    XMLTagAttribute typeAttr = XMLTagAttribute("type", std::string(this->evName));
    tag.AddAttribute(typeAttr);
    
    XMLTagAttribute timestampAttr = XMLTagAttribute("timestamp", std::to_string(this->timestamp));
    tag.AddAttribute(timestampAttr);

    if(
        evName == RECORD_LOW_FPS ||
        evName == RECORD_HIGH_FPS ||
        evName == RECORD_LOW_DIST ||
        evName == RECORD_HIGH_DIST
    ) {
        XMLTagAttribute record = XMLTagAttribute("record", std::to_string(this->record));
        tag.AddAttribute(record);
    }
    else if(evName == GENERAL_UPDATE) {
        XMLTagAttribute fpsAttr = XMLTagAttribute("fps", std::to_string(this->fps));
        tag.AddAttribute(fpsAttr);

        XMLTagAttribute distAttr = XMLTagAttribute("distance", std::to_string(this->distance));
        tag.AddAttribute(distAttr);

        XMLTagAttribute targetSeenAttr = XMLTagAttribute("targetSeen", (this->targetSeen ? "true" : "false"));
        tag.AddAttribute(targetSeenAttr);
    }

    return tag;
}

/**
 * Initalizes all values to their undefined states.
 */
void LogEvent::InitBlank(const std::string evName, long timestamp) {
    this->evName     = evName;
    this->timestamp  = timestamp;
    this->record     = -1;
    this->fps        = -1;
    this->distance   = -1;
    this->targetSeen = false;
}