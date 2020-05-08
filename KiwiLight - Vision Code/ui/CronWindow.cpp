#include "../KiwiLight.h"

/**
 * Source file for the CronWindow class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new CronWindow.
 */
CronWindow::CronWindow(GtkWindowType type) {
    this->window = Window(type, false);
        window.SetCSS("ui/Style.css");

        Panel contents = Panel(false, 5);
            Label header = Label("Cron Manager");
                header.SetName("header");
                contents.Pack_start(header.GetWidget(), true, true, 10);

            //evauate current KiwiLight-related crons and display the results
            std::vector<std::string> existingRules = ReadKiwiLightRules();

            std::string cronStatusString = "KiwiLight detected " + std::to_string(existingRules.size()) + " active rule";
            if(existingRules.size() != 1) {
                cronStatusString += "s";
            }
            cronStatusString += ".";

            this->cronStatus = Label(cronStatusString);
                cronStatus.SetName("subHeader");
                contents.Pack_start(cronStatus.GetWidget(), true, true, 5);

            //assemble a current file status. This will read somewhat like this:
            //Current file: /home/<usr>/file.xml: Will (not) run on startup.

            std::string currentStatusString = "There is no loaded configuration.";
            this->currentRuleStatus = Label(currentStatusString);
                contents.Pack_start(currentRuleStatus.GetWidget(), true, true, 5);

            std::string currentFile = KiwiLightApp::GetCurrentFile();
            if(currentFile != "") {
                currentStatusString = "Current File: " + currentFile + ": ";

                bool confRuleExists = isCurrentFileAutomatic();

                if(confRuleExists) {
                    currentStatusString += "Will run on startup.";
                } else {
                    currentStatusString += "Will not run on startup.";
                }

                currentRuleStatus.SetText(currentStatusString);

                //if there is a file loaded, generate the query with the buttons as well
                Label question = Label("Would you like this file to be run automatically when the computer starts up?");
                    contents.Pack_start(question.GetWidget(), true, true, 0);

                Panel buttonPanel = Panel(true, 0);
                    Button shouldRun = Button("Yes", KiwiLightApp::SaveConfigShouldRun);
                        buttonPanel.Pack_start(shouldRun.GetWidget(), true, true, 0);

                    Button shouldNotRun = Button("No", KiwiLightApp::SaveConfigShouldNotRun);
                        buttonPanel.Pack_start(shouldNotRun.GetWidget(), true, true, 0);

                    contents.Pack_start(buttonPanel.GetWidget(), true, true, 5);
            }

        window.SetPane(contents);

    this->cronwindow = window.GetWidget();
}

/**
 * Returns true if the window is being shown, and false otherwise.
 */
bool CronWindow::isOpen() {
    return this->isOpened;
}

/**
 * Shows the CronWindow.
 */
void CronWindow::Show() {
    window.Show();
}

/**
 * Destroys the CronWindow.
 */
void CronWindow::Close() {
    if(this->isOpened) {
        gtk_widget_destroy(this->cronwindow);
    }
}

/**
 * Saves in cron wether or not the current configuration should be run on boot or not.
 */
void CronWindow::SaveRule(bool shouldCurrentConfigRun) {
    std::cout << "Save Cron Rule: " << (shouldCurrentConfigRun ? "Config should run" : " Config should not run.") << std::endl;

    //generate new cron file
    std::vector<std::string> currentRules = ReadAllRules();
    bool fileRuleExists = isCurrentFileAutomatic();
    std::string currentFile = KiwiLightApp::GetCurrentFile();

    if(shouldCurrentConfigRun) {
        //current file should be configured to run on startup
        if(!fileRuleExists) {
            currentRules.push_back("@reboot KiwiLight -c " + currentFile);
        }
    } else {
        //current file should be configured to NOT run on startup
        for(int i=0; i<currentRules.size(); i++) {
            if(currentRules[i].find(currentFile) != std::string::npos) {
                currentRules.erase(currentRules.begin() + i);
            }
        }
    }

    //write new cron file in /home/<usr>/KiwiLightData/tmp/crontab.txt
    std::string fileLocation = "";
    char *home = getenv("HOME");
    if(home != NULL) {
        fileLocation = std::string(home) + "/KiwiLightData/tmp/crontab.txt";
    } else {
        std::cout << "CronWindow could not find HOME" << std::endl;
        return;
    }

    std::string toWrite = "";
    for(int i=0; i<currentRules.size(); i++) {
        toWrite += currentRules[i] + "\n";
    }

    std::ofstream file = std::ofstream(fileLocation, std::ofstream::out);
    file.write(toWrite.c_str(), toWrite.length());
    file.close();

    //install the crontab
    Shell::ExecuteCommand("crontab " + fileLocation);
}


void CronWindow::SetName(std::string name) {
    gtk_widget_set_name(this->cronwindow, name.c_str());
}


bool CronWindow::isCurrentFileAutomatic() {
    //evaluate wether or not this config is set to run on boot
    std::string currentFile = KiwiLightApp::GetCurrentFile();
    std::vector<std::string> existingRules = ReadKiwiLightRules();
    for(int i=0; i<existingRules.size(); i++) {
        if(existingRules[i].find(currentFile) != std::string::npos) {
            return true;
        }
    }

    return false;
}


std::vector<std::string> CronWindow::ReadAllRules() {
    std::string output = Shell::ExecuteCommand("crontab -l");
    return StringUtils::SplitString(output, '\n');
}


std::vector<std::string> CronWindow::ReadKiwiLightRules() {
    std::vector<std::string> validRules;
    std::vector<std::string> rules = ReadAllRules();
    for(int i=0; i<rules.size(); i++) {
        std::string rule = rules[i];
        if(StringUtils::StringStartsWith(rule, "@reboot")) {
            std::vector<std::string> segments = StringUtils::SplitString(rule, ' ');
            if(segments.size() >= 2) {
                if(segments[1] == "KiwiLight") {
                    validRules.push_back(rule);
                }
            }
        }
    }

    return validRules;
}