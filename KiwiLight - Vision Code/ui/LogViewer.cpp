#include "../KiwiLight.h"
#include "gnuplot_i.hpp"

#define DATA_PLOT FALSE //This flag enables (or disables) a feature offered by the LogViewer that plots the 
                        //log data over time, which can be useful for debugging performance issues. However,
                        //the plotter is unstable and may crash frequently for a reason that is still unknown.
                        //Set this flag to TRUE to enable, and FALSE to disable.

/**
 * Source file for the LogViewer class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new LogViewer displaying the contents of "log"
 */
LogViewer::LogViewer(XMLDocument log) {
    std::string fileName = log.FileName();
    this->window = Window(GTK_WINDOW_TOPLEVEL, false);
        this->contents = Panel(false, 0);

            Label header = Label("Log");
                header.SetName("header");
                contents.Pack_start(header.GetWidget(), false, false, 0);

            XMLTag logTag = log.GetTagsByName("KiwiLightLog")[0];
            Panel logInfoContainer = Panel(true, 0);
                Panel logInfoPanel = Panel(false, 5);
                    Panel nameAndRecordedPanel = Panel(true, 0);
                        //Conf names readout plus separator
                        Label confNamesHeader = Label("Configuration Names: ");
                            confNamesHeader.SetName("gray");
                            nameAndRecordedPanel.Pack_start(confNamesHeader.GetWidget(), false, false, 0);

                        std::string names = logTag.GetAttributesByName("confnames")[0].Value();
                        this->confNames = Label(names);
                            nameAndRecordedPanel.Pack_start(confNames.GetWidget(), false, false, 0);

                        Separator sep2 = Separator(false);
                            nameAndRecordedPanel.Pack_start(sep2.GetWidget(), false, false, 5);

                        //time readout but NO separator
                        Label timeHeader = Label("Recorded: ");
                            timeHeader.SetName("gray");
                            nameAndRecordedPanel.Pack_start(timeHeader.GetWidget(), false, false, 0);

                        logInfoPanel.Pack_start(nameAndRecordedPanel.GetWidget(), false, false, 0);

                    std::string time = logTag.GetAttributesByName("started")[0].Value();
                    //doctor the time string. Instead of MM-DD-YYYY-HH-MM-SS we want MM/DD/YYYY - HH:MM:SS
                    std::vector<std::string> timeSegments = Util::SplitString(time, '-');
                    std::string newTime =
                        timeSegments[0] + "/" + timeSegments[1] + "/" + timeSegments[2] + " - " + timeSegments[3] + ":" + timeSegments[4] + ":" + timeSegments[5];

                    this->logRecordedTime = Label(newTime);
                        nameAndRecordedPanel.Pack_start(logRecordedTime.GetWidget(), false, false, 0);

                    Panel confFilesPanel = Panel(true, 0);
                        //Conf file path readout plus separator
                        Label confFilesHeader = Label("Configuration Files: ");
                            confFilesHeader.SetName("gray");
                            confFilesPanel.Pack_start(confFilesHeader.GetWidget(), false, false, 0);

                        std::string files = logTag.GetAttributesByName("conffiles")[0].Value();
                        this->confFiles = Label(files);
                            confFilesPanel.Pack_start(confFiles.GetWidget(), false, false, 0);

                        logInfoPanel.Pack_start(confFilesPanel.GetWidget(), false, false, 0);

                    logInfoContainer.Pack_start(logInfoPanel.GetWidget(), true, false, 0);

                contents.Pack_start(logInfoContainer.GetWidget(), false, false, 10);

            Separator sep3 = Separator(true);
                contents.Pack_start(sep3.GetWidget(), false, false, 0);

            //parse ints directly in the file
            this->totalFramesNum          = Util::toInt(logTag.GetTagsByName("TotalFrames")[0].Content(), 1),
            this->framesWithTargetSeenNum = Util::toInt(logTag.GetTagsByName("FramesWithTargetSeen")[0].Content(), 1);

            //parse double directly in the file
            this->averageFPSNum = Util::toDouble(logTag.GetTagsByName("AverageFPS")[0].Content(), 1),
            this->averageDistanceNum  = Util::toDouble(logTag.GetTagsByName("AverageDistance")[0].Content(), 1);

            //declare numbers located in events
            this->targetLostEventCountNum = 0,
            this->closestDistanceNum = 0,
            this->farthestDistanceNum = 0;

            this->fastestFPSNum = 0,
            this->slowestFPSNum = 0;

            //parse events
            std::vector<XMLTag> eventTags = logTag.GetTagsByName("Events")[0].GetTagsByName("Event");
            this->numEvents = eventTags.size();
            this->events = new LogEvent[(const int) numEvents];
            bool lastTargetSeen = false;
            for(int i=0; i<numEvents; i++) {
                LogEvent event = eventFromTag(eventTags[i]);
                if(event.GetTimestamp() < 0) { //invalid event
                    continue;
                }

                events[i] = event;

                if(event.GetEventType() == LogEvent::RECORD_HIGH_FPS)  { fastestFPSNum = event.GetRecord();       }
                if(event.GetEventType() == LogEvent::RECORD_LOW_FPS)   { slowestFPSNum = event.GetRecord();       }
                if(event.GetEventType() == LogEvent::RECORD_HIGH_DIST) { farthestDistanceNum = event.GetRecord(); }
                if(event.GetEventType() == LogEvent::RECORD_LOW_DIST)  { closestDistanceNum = event.GetRecord();  }
                if(event.GetEventType() == LogEvent::GENERAL_UPDATE) {
                    if(lastTargetSeen && !event.GetTargetSeen()) {
                        targetLostEventCountNum++;
                    }
                    lastTargetSeen = event.GetTargetSeen();
                }
            }

            Panel body = Panel(true, 0);
                this->readouts = Panel(false, 0);
                    body.Pack_start(readouts.GetWidget(), true, false, 0);

                contents.Pack_start(body.GetWidget(), false, false, 0);

            //total running time readout
            this->totalRunningTimeNum = totalFramesNum * (1000 / averageFPSNum);
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
            createHorizontalReadout("Average Frame Rate: ", averageFrameTime, true);

            //fastest frame time readout
            std::string fastestFrameTimeString = std::to_string(fastestFPSNum) + " FPS";
            this->fastestFrameTime = Label(fastestFrameTimeString);
            createHorizontalReadout("Fastest Frame Rate: ", fastestFrameTime, false);

            //slowest frame time readout
            std::string slowestFrameTimeString = std::to_string(slowestFPSNum) + " FPS";
            this->slowestFrameTime = Label(slowestFrameTimeString);
            createHorizontalReadout("Slowest Frame Rate: ", slowestFrameTime, false);

            bool distUnknown = (closestDistanceNum == DBL_MAX && farthestDistanceNum == 0);

            //average distance readout
            this->averageDistance = Label(distUnknown ? "Unknown" : std::to_string(averageDistanceNum));
            createHorizontalReadout("Average Distance: ", averageDistance, true);

            //closest distance readout
            this->closestDistance = Label(distUnknown ? "Unknown" : std::to_string(closestDistanceNum));
            createHorizontalReadout("Closest Distance: ", closestDistance, false);

            //farthest distance readout
            this->farthestDistance = Label(distUnknown ? "Unknown" : std::to_string(farthestDistanceNum));
            createHorizontalReadout("Farthest Distance: ", farthestDistance, false);

            #if DATA_PLOT == TRUE
            //show plot button (only if DATA_PLOT is enabled)
            this->showingPlot = false;
            this->plotButton = Button("Show Plot", KiwiLightApp::ToggleLogPlot);
                readouts.Pack_start(plotButton.GetWidget(), true, false, 0);
            #endif

            this->window.SetPane(contents);
        this->window.SetCSS("ui/Style.css");
    this->widget = this->window.GetWidget();
    this->initalized = true;
}

/**
 * Shows the LogViewer.
 */
void LogViewer::Show() {
    this->window.Show();
}

/**
 * Turns on/off the log Gnuplot, which shows seen, FPS, and Distance data.
 */
void LogViewer::TogglePlotShowing() {
    showingPlot = !showingPlot;

    if(showingPlot) {
        g_thread_new("log plotter", GThreadFunc(KiwiLightApp::GenerateLogPlot), NULL);
    }

    //set text of plot button
    plotButton.SetText(showingPlot ? "Hide Plot" : "Show Plot");
}

/**
 * Generates and shows the log Gnuplot.
 */
void LogViewer::GenerateAndShowPlot() {
    int elapsedTimeMS = totalFramesNum * (1000 / averageFPSNum);
    int maxFPS = (int) fastestFPSNum;
    double maxDist = farthestDistanceNum;
    generatePlot(elapsedTimeMS, maxFPS, maxDist, events, (const int) numEvents);
}

/**
 * Should be called when the LogViewer is destroyed to prevent memory leaks.
 * Deletes the events array.
 */
void LogViewer::Release() {
    if(this->initalized) {
        delete[] events;
    }
}

/**
 * Creates a horizontal readout.
 * @param header The name of the value being read out.
 * @param readout The Label displaying the value.
 * @param isBig True if the readout should be big, false for normal size.
 */
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

        readouts.Pack_start(readoutPanel.GetWidget(), false, false, pad);
}

/**
 * Generates two graphs: one for FPS, one for Distance, and returns an opencv Mat with the image.
 */
void LogViewer::generatePlot(long elapsedTime, int maxFPS, double maxDist, LogEvent events[], const int numEvents) {
    Gnuplot plot;
    
    maxFPS *= 1.1;
    maxDist *= 1.1;
    
    //resolve the height of the background of the graph
    int imageHeight = (maxFPS > maxDist ? maxFPS : maxDist);
    
    if(maxDist == 0) {
        imageHeight = maxFPS;
    }
    
    int elapsedTimeSeconds = elapsedTime / 1000;
    
    //configure plot legend and color palette
    plot.set_legend("outside right top");
    plot << "set palette model RGB defined (0 0.7 1 0.7, 1 1 0.7 0.7)";
    plot.set_cbrange(0, 1);

    //declare image for distance plot. This image will show all times where a target was spotted.
    unsigned char 
        imageSamples[elapsedTimeSeconds * imageHeight];

    //parse the events
    std::vector<double>
        fpsTimes,
        distanceTimes;

    std::vector<double>
        fpsReadings,
        distanceReadings;

    //assemble vectors for plotting points
    int lastGUTimestampSeconds = 0;
    for(int i=0; i<numEvents; i++) {
        try {
            double timestamp = events[i].GetTimestamp() / 1000.0;
            if(events[i].GetEventType() == LogEvent::RECORD_HIGH_FPS || events[i].GetEventType() == LogEvent::RECORD_LOW_FPS) {
                fpsTimes.push_back(timestamp);
                fpsReadings.push_back(events[i].GetRecord());
            } else if(events[i].GetEventType() == LogEvent::RECORD_HIGH_DIST || events[i].GetEventType() == LogEvent::RECORD_LOW_DIST) {
                distanceTimes.push_back(timestamp);
                distanceReadings.push_back(events[i].GetRecord());
            } else { //GENERAL_UPDATE
                fpsTimes.push_back(timestamp);
                fpsReadings.push_back(events[i].GetFPS());

                distanceTimes.push_back(timestamp);
                distanceReadings.push_back(events[i].GetDistance());

                int colorValue = (events[i].GetTargetSeen() ? 0 : 1);
                //fill in fps image
                for(int r=lastGUTimestampSeconds; r<timestamp; r++) {
                    for(int c=0; c<imageHeight; c++) {
                        imageSamples[(c * elapsedTimeSeconds) + r] = colorValue;
                    }
                }

                lastGUTimestampSeconds = timestamp;
            }
        } catch(std::bad_alloc ex) {
            std::cout << "bad_alloc encountered. Skipping event." << std::endl;
        }
    }

    //resolve plot height and apply
    std::vector<double> distanceWithoutNegatives = Util::RemoveOccurances(distanceReadings, -1);

    int fpsHeight = Util::MaxWithoutOutliers(fpsReadings, 30);
    int distHeight = Util::MaxWithoutOutliers(distanceWithoutNegatives, 150);
    int plotHeight = (fpsHeight > distHeight ? fpsHeight : distHeight);
    
    if(distHeight > 5000) {
        plotHeight = fpsHeight;
    }
    plot.set_xrange(0, elapsedTimeSeconds);
    plot.set_yrange(0, plotHeight);

    //plot image and points
    plot.plot_image(imageSamples, elapsedTimeSeconds, imageHeight, "Seen");
    plot.plot_xy(fpsTimes, fpsReadings, "FPS");
    plot.plot_xy(distanceTimes, distanceReadings, "Distance");

    plot.showonscreen();

    while(showingPlot) {
        usleep(250000);
    }
}

/**
 * Interprets the XMLTag tag and creates a LogEvent out of it.
 */
LogEvent LogViewer::eventFromTag(XMLTag tag) {
    try {
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
    } catch(std::invalid_argument ex) {
        std::cout << "Could not parse log event tag " << tag.ReturnString() << "." << std::endl;
        return LogEvent(LogEvent::GENERAL_UPDATE, -1, -1, -1, -1);
    }
}

/**
 * Takes a number of milliseconds and and returns a time.
 * Format: x hours, x minutes, x seconds
 */
std::string LogViewer::timeFromMS(long ms) {
    int seconds = (ms / 1000) % 60;
    int minutes = (ms / 60000) % 60;
    int hours = ms / 1440000;

    return std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";
}
