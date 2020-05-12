#include "UI.h"

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

                std::string names = logTag.GetAttributesByName("confname")[0].Value();
                this->confNames = Label(names);
                    logInfoPanel.Pack_start(confNames.GetWidget(), false, false, 0);

                Separator sep2 = Separator(false);
                    logInfoPanel.Pack_start(sep2.GetWidget(), false, false, 0);

                //Conf file path readout plus separator
                Label confFilesHeader = Label("Configuration Files: ");
                    confFilesHeader.SetName("gray");
                    logInfoPanel.Pack_start(confFilesHeader.GetWidget(), false, false, 0);

                std::string files = logTag.GetAttributesByName("conffilename")[0].Value();
                this->confFiles = Label(files);
                    logInfoPanel.Pack_start(confFiles.GetWidget(), false, false, 0);

                Separator sep3 = Separator(false);
                    logInfoPanel.Pack_start(sep3.GetWidget(), false, false, 0);

                //time readout but NO separator
                Label timeHeader = Label("Recorded: ");
                    timeHeader.SetName("gray");
                    logInfoPanel.Pack_start(timeHeader.GetWidget(), false, false, 0);

                std::string time = logTag.GetAttributesByName("time")[0].Value();
                //doctor the time string. Instead of MM-DD-YYYY-HH-MM-SS we want MM/DD/YYYY - HH:MM:SS
                std::vector<std::string> timeSegments = StringUtils::SplitString(time, '-');
                std::string newTime =
                    timeSegments[0] + "/" + timeSegments[1] + "/" + timeSegments[2] + " - " + timeSegments[3] + ":" + timeSegments[4] + ":" + timeSegments[5];

                this->logRecordedTime = Label(newTime);
                    logInfoPanel.Pack_start(logRecordedTime.GetWidget(), false, false, 0);

                contents.Pack_start(logInfoPanel.GetWidget(), false, false, 10);

            Separator ribbonSeparator = Separator(true);
                contents.Pack_start(ribbonSeparator.GetWidget(), false, false, 0);

            //numbers that we will be displaying
            int 
                totalFramesNum          = std::stoi(logTag.GetTagsByName("TotalFrames")[0].Content()),
                framesWithTargetSeenNum = std::stoi(logTag.GetTagsByName("FramesWithTargetSeen")[0].Content()),
                targetLostEventCountNum = std::stoi(logTag.GetTagsByName("TargetLostEventCount")[0].Content()),
                fastestFrameTimeNum     = std::stoi(logTag.GetTagsByName("FastestFrameTime")[0].Content()),
                slowestFrameTimeNum     = std::stoi(logTag.GetTagsByName("SlowestFrameTime")[0].Content()),
                closestDistanceNum      = std::stoi(logTag.GetTagsByName("ClosestDistance")[0].Content()),
                farthestDistanceNum     = std::stoi(logTag.GetTagsByName("FarthestDistance")[0].Content());

            double 
                averageFrameTimeNum = std::stod(logTag.GetTagsByName("AverageFrameTime")[0].Content()),
                averageDistanceNum  = std::stod(logTag.GetTagsByName("AverageDistance")[0].Content());

            //total running time readout
            int totalRunningTimeNum = totalFramesNum * averageFrameTimeNum;
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
            int avgFPS = (int) (1000 / averageFrameTimeNum);
            std::string avgFrameTimeString = std::to_string(averageFrameTimeNum) + " ms/frame (" + std::to_string(avgFPS) + " FPS)";
            this->averageFrameTime = Label(avgFrameTimeString);
            createHorizontalReadout("Average Frame Time: ", averageFrameTime, true);

            //fastest frame time readout
            int fastestFPS = 1000 / fastestFrameTimeNum;
            std::string fastestFrameTimeString = std::to_string(fastestFrameTimeNum) + " ms (" + std::to_string(fastestFPS) + " FPS)";
            this->fastestFrameTime = Label(fastestFrameTimeString);
            createHorizontalReadout("Fastest Frame Time: ", fastestFrameTime, false);

            //slowest frame time readout
            int slowestFPS = 1000 / slowestFrameTimeNum;
            std::string slowestFrameTimeString = std::to_string(slowestFrameTimeNum) + " ms (" + std::to_string(slowestFPS) + " FPS)";
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


std::string LogViewer::timeFromMS(long ms) {
    int seconds = ms / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;

    return std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";
}