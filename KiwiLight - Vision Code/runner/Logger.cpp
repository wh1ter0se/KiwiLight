#include "Runner.h"

/**
 * Source file for the Logger class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

Logger::Logger(std::string filePath) {
    this->filePath = filePath;
    this->confName = "Unavailable";
    this->confFilePath = "Unavailable";
    this->beginTime = "";
    this->totalFrames = 0;
    this->framesWithTargetSeen = 0;
    this->targetLostEventCount = 0;
    this->runningFrameTimeAvg = 0;
    this->fastestFrameTime = 10000000;
    this->slowestFrameTime = 0;
    this->runningDistanceAvg = 0;
    this->closestDistance = 10000000;
    this->farthestDistance = 0;
    this->lastFrameMessage = Runner::NULL_MESSAGE;
}


void Logger::SetConfName(std::string confName, std::string confFilePath) {
    this->confName = confName;
    this->confFilePath = confFilePath;
}


void Logger::Start() {
    beginTime = Clock::GetDateString();
    clock.Start();
}


void Logger::Log(std::string runnerOutput) {
    std::string slicedMessage = runnerOutput.substr(1, runnerOutput.length() - 1);
    std::vector<std::string> messageSegments = StringUtils::SplitString(slicedMessage, ',');

    totalFrames++;
    if(runnerOutput != Runner::NULL_MESSAGE) {
        framesWithTargetSeen++;
    }

    if(lastFrameMessage != Runner::NULL_MESSAGE && runnerOutput == Runner::NULL_MESSAGE) {
        targetLostEventCount++;
    }

    //calculate the frame time
    long frameTime = clock.GetTime();
    int timeBetweenFrames = frameTime - lastFrameTime;

    //store record times
    if(timeBetweenFrames < fastestFrameTime) {
        fastestFrameTime = timeBetweenFrames;
    }

    if(timeBetweenFrames > slowestFrameTime) {
        slowestFrameTime = timeBetweenFrames;
    }
    
    //calculate new frame time average
    double expandedFrameTimeAvg = runningFrameTimeAvg * (totalFrames - 1);
    expandedFrameTimeAvg += timeBetweenFrames;
    runningFrameTimeAvg = expandedFrameTimeAvg / totalFrames;

    //calculate distance running average and records
    int distance = std::stoi(messageSegments[4]);
    if(distance > -1) {
        double expandedDistanceAvg = runningDistanceAvg * (totalFrames - 1);
        expandedDistanceAvg += distance;
        runningDistanceAvg = expandedDistanceAvg / totalFrames;

        if(distance < closestDistance) {
            closestDistance = distance;
        }

        if(distance > farthestDistance) {
            farthestDistance = distance;
        }
    }

    //store prevs
    lastFrameMessage = runnerOutput;
    lastFrameTime = frameTime;

    //print it all into the file
    WriteNewFile();
}


void Logger::WriteNewFile() {
    XMLDocument document = XMLDocument();

        //<KiwiLightLog time="DD/MM/YY:HH:MM:SS">
        XMLTag logTag = XMLTag("KiwiLightLog");
            XMLTagAttribute date = XMLTagAttribute("time", beginTime);
                logTag.AddAttribute(date);

            XMLTagAttribute fileNameAttr = XMLTagAttribute("conffilename", confFilePath);
                logTag.AddAttribute(fileNameAttr);

            XMLTagAttribute confNameAttr = XMLTagAttribute("confname", confName);
                logTag.AddAttribute(confNameAttr);

            XMLTag totalFramesTag = XMLTag("TotalFrames", std::to_string(totalFrames));
                logTag.AddTag(totalFramesTag);

            XMLTag framesWithTargetSeenTag = XMLTag("FramesWithTargetSeen", std::to_string(framesWithTargetSeen));
                logTag.AddTag(framesWithTargetSeenTag);

            XMLTag targetLostEventCountTag = XMLTag("TargetLostEventCount", std::to_string(targetLostEventCount));
                logTag.AddTag(targetLostEventCountTag);

            XMLTag averageFrameTimeTag = XMLTag("AverageFrameTime", std::to_string(runningFrameTimeAvg));
                logTag.AddTag(averageFrameTimeTag);

            XMLTag fastestFrameTimeTag = XMLTag("FastestFrameTime", std::to_string(fastestFrameTime));
                logTag.AddTag(fastestFrameTimeTag);

            XMLTag slowestFrameTimeTag = XMLTag("SlowestFrameTime", std::to_string(slowestFrameTime));
                logTag.AddTag(slowestFrameTimeTag);

            XMLTag averageDistanceTag = XMLTag("AverageDistance", std::to_string(runningDistanceAvg));
                logTag.AddTag(averageDistanceTag);

            XMLTag closestDistanceTag = XMLTag("ClosestDistance", std::to_string(closestDistance));
                logTag.AddTag(closestDistanceTag);

            XMLTag farthestDistanceTag = XMLTag("FarthestDistance", std::to_string(farthestDistance));
                logTag.AddTag(farthestDistanceTag);

            document.AddTag(logTag);

    document.WriteFile(this->filePath);
}