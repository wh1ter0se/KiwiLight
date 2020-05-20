#include "../KiwiLight.h"
#include "gnuplot_i.hpp"

/**
 * Source file for the LogViewer class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


LogViewer::LogViewer(std::string fileName) {
    XMLDocument log = XMLDocument(fileName);

    this->window = Window(GTK_WINDOW_TOPLEVEL, false);
        this->contents = Panel(false, 0);

            Label header = Label("Log");
                header.SetName("header");
                contents.Pack_start(header.GetWidget(), false, false, 0);

            XMLTag logTag = log.GetTagsByName("KiwiLightLog")[0];
            Panel logInfoPanel = Panel(true, 5);
                // Log file path readout plus separator
                Label logFileHeader = Label("Log File: ");
                    logFileHeader.SetName("gray");
                    logInfoPanel.Pack_start(logFileHeader.GetWidget(), false, false, 0);

                this->logFileName = Label(fileName);
                    logInfoPanel.Pack_start(logFileName.GetWidget(), false, false, 0);

                Separator sep1 = Separator(false);
                    logInfoPanel.Pack_start(sep1.GetWidget(), false, false, 0);

                //Conf names readout plus separator
                Label confNamesHeader = Label("Configuration Names: ");
                    confNamesHeader.SetName("gray");
                    logInfoPanel.Pack_start(confNamesHeader.GetWidget(), false, false, 0);

                std::string names = logTag.GetAttributesByName("confnames")[0].Value();
                this->confNames = Label(names);
                    logInfoPanel.Pack_start(confNames.GetWidget(), false, false, 0);

                Separator sep2 = Separator(false);
                    logInfoPanel.Pack_start(sep2.GetWidget(), false, false, 0);

                //Conf file path readout plus separator
                Label confFilesHeader = Label("Configuration Files: ");
                    confFilesHeader.SetName("gray");
                    logInfoPanel.Pack_start(confFilesHeader.GetWidget(), false, false, 0);

                std::string files = logTag.GetAttributesByName("conffiles")[0].Value();
                this->confFiles = Label(files);
                    logInfoPanel.Pack_start(confFiles.GetWidget(), false, false, 0);

                Separator sep3 = Separator(false);
                    logInfoPanel.Pack_start(sep3.GetWidget(), false, false, 0);

                //time readout but NO separator
                Label timeHeader = Label("Recorded: ");
                    timeHeader.SetName("gray");
                    logInfoPanel.Pack_start(timeHeader.GetWidget(), false, false, 0);

                std::string time = logTag.GetAttributesByName("started")[0].Value();
                //doctor the time string. Instead of MM-DD-YYYY-HH-MM-SS we want MM/DD/YYYY - HH:MM:SS
                std::vector<std::string> timeSegments = StringUtils::SplitString(time, '-');
                std::string newTime =
                    timeSegments[0] + "/" + timeSegments[1] + "/" + timeSegments[2] + " - " + timeSegments[3] + ":" + timeSegments[4] + ":" + timeSegments[5];

                this->logRecordedTime = Label(newTime);
                    logInfoPanel.Pack_start(logRecordedTime.GetWidget(), false, false, 0);

                contents.Pack_start(logInfoPanel.GetWidget(), false, false, 10);

            Separator ribbonSeparator = Separator(true);
                contents.Pack_start(ribbonSeparator.GetWidget(), false, false, 0);

            //parse ints directly in the file
            int 
                totalFramesNum          = std::stoi(logTag.GetTagsByName("TotalFrames")[0].Content()),
                framesWithTargetSeenNum = std::stoi(logTag.GetTagsByName("FramesWithTargetSeen")[0].Content());

            //parse double directly in the file
            double 
                averageFPSNum = std::stod(logTag.GetTagsByName("AverageFPS")[0].Content()),
                averageDistanceNum  = std::stod(logTag.GetTagsByName("AverageDistance")[0].Content());

            //declare numbers located in events
            int
                targetLostEventCountNum = 0,
                closestDistanceNum = 0,
                farthestDistanceNum = 0;

            double 
                fastestFPSNum = 0,
                slowestFPSNum = 0;

            //parse events
            std::vector<XMLTag> eventTags = logTag.GetTagsByName("Events")[0].GetTagsByName("Event");
            const int numEvents = (const int) eventTags.size();
            LogEvent events[numEvents];
            bool lastTargetSeen = false;
            for(int i=0; i<numEvents; i++) {
                LogEvent event = eventFromTag(eventTags[i]);
                events[i] = event;

                if(event.GetEventType() == LogEvent::RECORD_HIGH_FPS)  { fastestFPSNum = event.GetRecord();  }
                if(event.GetEventType() == LogEvent::RECORD_LOW_FPS)   { slowestFPSNum = event.GetRecord();   }
                if(event.GetEventType() == LogEvent::RECORD_HIGH_DIST) { farthestDistanceNum = event.GetRecord(); }
                if(event.GetEventType() == LogEvent::RECORD_LOW_DIST)  { closestDistanceNum = event.GetRecord();  }
                if(event.GetEventType() == LogEvent::GENERAL_UPDATE) {
                    if(lastTargetSeen && !event.GetTargetSeen()) {
                        targetLostEventCountNum++;
                    }
                    lastTargetSeen = event.GetTargetSeen();
                }
            }

            int totalRunningTimeNum = totalFramesNum * averageFPSNum;
            generateGraph(totalRunningTimeNum, fastestFPSNum, farthestDistanceNum, events, numEvents);


            //total running time readout
            this->totalRunningTime = Label(timeFromMS(totalRunningTimeNum));
            createHorizontalReadout("Running Time: ", this->totalRunningTime, true);

            //total frames readout
            this->totalFrames = Label(std::to_string(totalFramesNum));
            createHorizontalReadout("Total Frames: ", totalFrames, false);

            //frames with target seen readout
            int percentageTargetSeen = (framesWithTargetSeenNum / (double) totalFramesNum) * 100;
            std::string seenFramesString = std::to_string(framesWithTargetSeenNum) + " (" + std::to_string(percentageTargetSeen) + "%)";
            this->framesWithTargetSeen = Label(seenFramesString);
            createHorizontalReadout("Frames With Target Seen: ", framesWithTargetSeen, false);

            //target lost event count readout
            this->targetLostEventCount = Label(std::to_string(targetLostEventCountNum));
            createHorizontalReadout("Number of times the target was lost: ", targetLostEventCount, false);

            //average frame time readout
            std::string avgFrameTimeString = std::to_string(averageFPSNum) + " FPS";
            this->averageFrameTime = Label(avgFrameTimeString);
            createHorizontalReadout("Average Frame Time: ", averageFrameTime, true);

            //fastest frame time readout
            std::string fastestFrameTimeString = std::to_string(fastestFPSNum) + " FPS";
            this->fastestFrameTime = Label(fastestFrameTimeString);
            createHorizontalReadout("Fastest Frame Time: ", fastestFrameTime, false);

            //slowest frame time readout
            std::string slowestFrameTimeString = std::to_string(slowestFPSNum) + " FPS";
            this->slowestFrameTime = Label(slowestFrameTimeString);
            createHorizontalReadout("Slowest Frame Time: ", slowestFrameTime, false);

            //average distance readout
            this->averageDistance = Label(std::to_string(averageDistanceNum));
            createHorizontalReadout("Average Distance: ", averageDistance, true);

            //closest distance readout
            this->closestDistance = Label(std::to_string(closestDistanceNum));
            createHorizontalReadout("Closest Distance: ", closestDistance, false);

            //farthest distance readout
            this->farthestDistance = Label(std::to_string(farthestDistanceNum));
            createHorizontalReadout("Farthest Distance: ", farthestDistance, false);

            this->window.SetPane(contents);
        this->window.SetCSS("ui/Style.css");
    this->logviewer = this->window.GetWidget();
}


void LogViewer::Show() {
    this->window.Show();
}


void LogViewer::SetName(std::string name) {
    gtk_widget_set_name(this->logviewer, name.c_str());
}


void LogViewer::createHorizontalReadout(std::string header, Label readout, bool isBig) {
    int pad = (isBig ? 10 : 0);

    Panel readoutPanel = Panel(true, 0);
        Label headerLabel = Label(header);
            headerLabel.SetName("gray");
            readoutPanel.Pack_start(headerLabel.GetWidget(), false, false, pad);
        
        readoutPanel.Pack_start(readout.GetWidget(), false, false, pad);

        if(isBig) {
            headerLabel.SetName("subHeader");
            readoutPanel.SetName("bigGreen");
        }

        contents.Pack_start(readoutPanel.GetWidget(), false, false, pad);
}

/**
 * Generates two graphs: one for FPS, one for Distance, and returns an opencv Mat with the image.
 */
Mat LogViewer::generateGraph(long elapsedTime, int maxFPS, int maxDist, LogEvent *events, int numEvents) {
    Gnuplot
        fpsPlot,
        distancePlot;

    int elapsedTimeSeconds = elapsedTime / 1000;

    fpsPlot.set_legend("outside right top");
    fpsPlot.set_xrange(0, elapsedTimeSeconds);
    fpsPlot.set_yrange(0, maxFPS);
    fpsPlot << "set palette model RGB defined (0 0.7 1 0.7, 1 1 0.7 0.7)";
    fpsPlot.set_cbrange(0, 1);

    distancePlot.set_legend("outside right top");
    distancePlot.set_xrange(0, elapsedTimeSeconds);
    distancePlot.set_yrange(0, maxDist);
    distancePlot << "set palette model RGB defined (0 0.7 1 0.7, 1 1 0.7 0.7)";
    distancePlot.set_cbrange(0, 1);

    //declare image for distance plot
    int imageHeight = (maxFPS > maxDist ? maxFPS : maxDist);
    unsigned char 
        imageSamples[elapsedTimeSeconds * maxFPS];

    //parse the events
    std::vector<double>
        fpsTimes,
        distanceTimes;

    std::vector<double>
        fpsReadings,
        distanceReadings;

    int lastGUTimestampSeconds = 0;
    for(int i=0; i<numEvents; i++) {
        LogEvent event = events[i];
        double timestamp = event.GetTimestamp() / 1000.0;
        if(event.GetEventType() == LogEvent::RECORD_HIGH_FPS || event.GetEventType() == LogEvent::RECORD_LOW_FPS) {
            fpsTimes.push_back(timestamp);
            fpsReadings.push_back(event.GetRecord());
        } else if(event.GetEventType() == LogEvent::RECORD_HIGH_DIST || event.GetEventType() == LogEvent::RECORD_LOW_DIST) {
            distanceTimes.push_back(timestamp);
            distanceReadings.push_back(event.GetRecord());
        } else { //GENERAL_UPDATE
            fpsTimes.push_back(timestamp);
            fpsReadings.push_back(event.GetFPS());

            distanceTimes.push_back(timestamp);
            distanceReadings.push_back(event.GetDistance());

            int colorValue = (event.GetTargetSeen() ? 0 : 1);
            //fill in fps image
            for(int r=lastGUTimestampSeconds; r<timestamp; r++) {
                for(int c=0; c<imageHeight; c++) {
                    imageSamples[(c * elapsedTimeSeconds) + r] = colorValue;
                }
            }

            lastGUTimestampSeconds = timestamp;
        }
    }

    fpsPlot.plot_image(imageSamples, elapsedTimeSeconds, maxFPS, "Seen");
    distancePlot.plot_image(imageSamples, elapsedTimeSeconds, maxFPS, "Seen");

    fpsPlot.plot_xy(fpsTimes, fpsReadings, "FPS");
    distancePlot.plot_xy(distanceTimes, distanceReadings, "Distance");

    fpsPlot.showonscreen();
    distancePlot.showonscreen();

    std::cin.get();

    return Mat();
}


LogEvent LogViewer::eventFromTag(XMLTag tag) {
    std::string type = tag.GetAttributesByName("type")[0].Value();
    long timestamp = std::stol(tag.GetAttributesByName("timestamp")[0].Value());
    if(type == LogEvent::GENERAL_UPDATE) {
        double fps = std::stod(tag.GetAttributesByName("fps")[0].Value());
        int distance = std::stoi(tag.GetAttributesByName("distance")[0].Value());
        bool targetSeen = (tag.GetAttributesByName("targetSeen")[0].Value() == "true" ? true : false);

        LogEvent event = LogEvent(type, timestamp, fps, distance, targetSeen);
        return event;
    } else {
        double record = std::stod(tag.GetAttributesByName("record")[0].Value());
        LogEvent event = LogEvent(type, timestamp, record);
        return event;
    }
}


std::string LogViewer::timeFromMS(long ms) {
    int seconds = ms / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;

    return std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";
}