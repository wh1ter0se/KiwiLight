#include "Runner.h"

/**
 * Source file for the Logger class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

const int Logger::UPDATE_VECTOR_MAX_SIZE = 250;
const int Logger::FILE_WRITE_INTERVAL = 2500;

/**
 * Creates a new Logger object. Output will be directed towards filePath.
 */
Logger::Logger(std::string filePath) {
    this->filePath = filePath;
    this->confName = "Unavailable";
    this->confFilePath = "Unavailable";
    this->beginTime = "";
    this->totalFrames = 0;
    this->framesWithTargetSeen = 0;
    this->runningFPSAvg = 0;
    this->runningDistanceAvg = 0;
    this->lastFrameMessage = Runner::NULL_MESSAGE;

    this->fastestFPSEvent = LogEvent(LogEvent::RECORD_HIGH_FPS, 0L, 0.0);
    this->slowestFPSEvent = LogEvent(LogEvent::RECORD_LOW_FPS, 0, DBL_MAX);
    this->closestDistanceEvent = LogEvent(LogEvent::RECORD_LOW_DIST, 0, DBL_MAX);
    this->farthestDistanceEvent = LogEvent(LogEvent::RECORD_HIGH_DIST, 0, 0.0);

    this->lastGeneralUpdateTime = 0;
    this->generalUpdateInterval = 125; //milliseconds
    this->lastFileWriteTime = 0;
}

/**
 * Sets the name of the configuration being logged.
 */
void Logger::SetConfName(std::string confName, std::string confFilePath) {
    this->confName = confName;
    this->confFilePath = confFilePath;
}

/**
 * Sets the clock so that timestamps are accurite.
 */
void Logger::Start() {
    beginTime = Clock::GetDateString();
    clock.Start();
}

/**
 * Logs one Runner frame.
 * @param runnerOutput The value returned by Runner::Iterate()
 */
void Logger::Log(std::string runnerOutput) {
    std::string slicedOutput = runnerOutput.substr(1, runnerOutput.length() - 1);
    std::vector<std::string> segments = Util::SplitString(slicedOutput, ',');

    totalFrames++;
    bool isTargetSeen = segments[1] != "-1";
    if(isTargetSeen) {
        framesWithTargetSeen++;
    }

    //calculate FPS for this frame
    long thisFrameTime = clock.GetTime();
    int timeSinceLastFrame = thisFrameTime - lastFrameTime;
    double thisFrameFPS = 1000 / (double) timeSinceLastFrame;

    //calculate new average FPS
    double expandedFPS = runningFPSAvg * (totalFrames - 1);
    expandedFPS += thisFrameFPS;
    runningFPSAvg = expandedFPS / totalFrames;

    //store events
    if(thisFrameFPS > fastestFPSEvent.GetRecord()) {
        fastestFPSEvent = LogEvent(LogEvent::RECORD_HIGH_FPS, thisFrameTime, thisFrameFPS);
    }

    if(thisFrameFPS < slowestFPSEvent.GetRecord()) {
        slowestFPSEvent = LogEvent(LogEvent::RECORD_LOW_FPS, thisFrameTime, thisFrameFPS);
    }

    //get distance
    int distance = std::stoi(segments[5]);
    if(distance > 0) {
        if(distance > farthestDistanceEvent.GetRecord()) {
            farthestDistanceEvent = LogEvent(LogEvent::RECORD_HIGH_DIST, thisFrameTime, distance);
        }

        if(distance < closestDistanceEvent.GetRecord()) {
            closestDistanceEvent = LogEvent(LogEvent::RECORD_LOW_DIST, thisFrameTime, distance);
        }
        
        double expandedDistance = runningDistanceAvg * (totalFrames - 1);
        expandedDistance += distance;
        runningDistanceAvg = expandedDistance / totalFrames;
    }

    //do we have to do a general update?
    if(thisFrameTime - lastGeneralUpdateTime >= generalUpdateInterval) {
        //perform a general update
        LogEvent update = LogEvent(LogEvent::GENERAL_UPDATE, thisFrameTime, thisFrameFPS, distance, isTargetSeen);
        updates.push_back(update);
        lastGeneralUpdateTime = thisFrameTime;
    }

    //do we need to cut back on the updates?
    if(updates.size() > UPDATE_VECTOR_MAX_SIZE) {
        //remove every other update event, and multiply the update interval by 2.
        for(int i=0; i<updates.size(); i++) {
            updates.erase(updates.begin() + i);
        }

        generalUpdateInterval *= 2;
    }

    if(thisFrameTime - lastFileWriteTime > FILE_WRITE_INTERVAL) {        
        WriteNewFile();
        lastFileWriteTime = thisFrameTime;
    }
    
    //store previous variables
    lastFrameTime = thisFrameTime;
}

/**
 * Writes all log info to the Logger's file path.
 */
void Logger::WriteNewFile() {
    XMLDocument document = XMLDocument();

    XMLTag KiwiLightLog = XMLTag("KiwiLightLog");
        XMLTagAttribute started = XMLTagAttribute("started", beginTime);
            KiwiLightLog.AddAttribute(started);

        XMLTagAttribute confnames = XMLTagAttribute("confnames", confName);
            KiwiLightLog.AddAttribute(confnames);

        XMLTagAttribute conffiles = XMLTagAttribute("conffiles", confFilePath);
            KiwiLightLog.AddAttribute(conffiles);

        /**
         * Tags to generate separate from events:
         * totalFrames
         * averageFPS
         * averageDistance
         */
        
        XMLTag TotalFrames = XMLTag("TotalFrames", std::to_string(totalFrames));
            KiwiLightLog.AddTag(TotalFrames);

        XMLTag FramesWithTargetSeen = XMLTag("FramesWithTargetSeen", std::to_string(framesWithTargetSeen));
            KiwiLightLog.AddTag(FramesWithTargetSeen);

        XMLTag AverageFPS = XMLTag("AverageFPS", std::to_string(runningFPSAvg));
            KiwiLightLog.AddTag(AverageFPS);

        XMLTag AverageDistance = XMLTag("AverageDistance", std::to_string(runningDistanceAvg));
            KiwiLightLog.AddTag(AverageDistance);

        XMLTag Events = XMLTag("Events");

            //add the records first
            Events.AddTag(fastestFPSEvent.EncodeXMLTag());
            Events.AddTag(slowestFPSEvent.EncodeXMLTag());
            Events.AddTag(farthestDistanceEvent.EncodeXMLTag());
            Events.AddTag(closestDistanceEvent.EncodeXMLTag());

            //add all general update tags
            for(int i=0; i<updates.size(); i++) {
                Events.AddTag(updates[i].EncodeXMLTag());
            }

            KiwiLightLog.AddTag(Events);

        document.AddTag(KiwiLightLog);

    document.WriteFile(this->filePath);
    
    std::string backupPath = this->filePath + ".bkp";
    document.WriteFile(backupPath);
}
