#include "../KiwiLight.h"

/**
 * Source file for the CronWindow class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new CronWindow with the passed window type
 */
CronWindow::CronWindow(GtkWindowType type) {
    this->window = Window(type, false);
        window.SetCSS("ui/Style.css");

        Panel contents = Panel(false, 5);
            Label header = Label("Auto-Start");
                header.SetName("header");
                contents.Pack_start(header.GetWidget(), false, false, 10);

            this->cronStatus = Label("Could not determine number of rules");
                cronStatus.SetName("subHeader");
                contents.Pack_start(cronStatus.GetWidget(), false, false, 5);

            this->tableContainer = Panel(true, 0);
                contents.Pack_start(tableContainer.GetWidget(), false, false, 0);

            renderTable();

            Button addButton = Button("Add", KiwiLightApp::CronAddButtonPressed);
                contents.Pack_start(addButton.GetWidget(), false, false, 0);

        window.SetPane(contents);

    this->widget = window.GetWidget();
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
        gtk_widget_destroy(this->widget);
    }
}

/**
 * Returns the cron rule that corresponds to line.
 * @param line the line number of the rule to return.
 */
std::string CronWindow::GetRuleByLine(int line) {
    return ReadKiwiLightRules()[line];
}

/**
 * Returns the name of the file that the passed rule will start.
 * @param rule A correctly formatted KiwiLight cron rule.
 * @return The location of the file that the rule will automatically start
 */
std::string CronWindow::GetFileNameFromRule(std::string rule) {
    return Util::SplitString(rule, ' ')[3];
}

/**
 * Saves in cron wether or not the current configuration should be run on boot or not.
 */
void CronWindow::SaveRule(std::string configFile, bool shouldRun) {
    //generate new cron file
    std::vector<std::string> currentRules = ReadAllRules();
    bool fileRuleExists = isFileAutomatic(configFile);

    if(shouldRun) {
        //current file should be configured to run on startup
        if(!fileRuleExists) {
            currentRules.push_back("@reboot KiwiLight -c " + configFile);
        } else { //rule does exist (redundant)
            //alert user
            ConfirmationDialog alert = ConfirmationDialog("File already added!");
            alert.ShowAndGetResponse();
        }
    } else {
        //current file should be configured to NOT run on startup
        for(int i=0; i<currentRules.size(); i++) {
            if(currentRules[i].find(configFile) != std::string::npos) {
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

    //destroy and re-render table
    gtk_container_remove(GTK_CONTAINER(this->tableContainer.GetWidget()), this->table.GetWidget());
    renderTable();
}

/**
 * Renders the table with the cron rule options.
 */
void CronWindow::renderTable() {
    std::vector<std::string> existingRules = ReadKiwiLightRules();
    std::string cronStatusString = "KiwiLight detected " + std::to_string(existingRules.size()) + " active rule";
    if(existingRules.size() != 1) {
        cronStatusString += "s";
    }
    cronStatusString += ".";
    cronStatus.SetText(cronStatusString);

    /**
     * Create a table for all crontab rules.
     * Each crontab rule will have its own row with it's name and a button to remove it.
     */
    this->table = Panel(false, 0);
        for(int i=0; i<existingRules.size(); i++) {
            //read the rule
            std::string fileName = GetFileNameFromRule(existingRules[i]);

            Panel row = Panel(true, 5);
                Label nameLabel = Label(fileName);
                    row.Pack_start(nameLabel.GetWidget(), false, false, 5);

                Button deactivateButton = Button("Remove", KiwiLightApp::CronDeleteButtonPressed, GINT_TO_POINTER(i));
                    row.Pack_start(deactivateButton.GetWidget(), false, false, 5);

                table.Pack_start(row.GetWidget(), false, false, 0);
        }
        tableContainer.Pack_start(table.GetWidget(), true, false, 0);

    //show all widgets
    gtk_widget_show_all(tableContainer.GetWidget());
}

/**
 * Returns true if the passed file is automatic, false otherwise.
 */
bool CronWindow::isFileAutomatic(std::string file) {
    //evaluate wether or not this config is set to run on boot
    std::vector<std::string> existingRules = ReadKiwiLightRules();
    for(int i=0; i<existingRules.size(); i++) {
        if(existingRules[i].find(file) != std::string::npos) {
            return true;
        }
    }

    return false;
}

/**
 * Reads and returns all existing system Cron rules.
 */
std::vector<std::string> CronWindow::ReadAllRules() {
    std::string output = Shell::ExecuteCommand("crontab -l");
    return Util::SplitString(output, '\n');
}

/**
 * Reads all existing system Cron rules and returns the KiwiLight-related ones.
 */
std::vector<std::string> CronWindow::ReadKiwiLightRules() {
    std::vector<std::string> validRules;
    std::vector<std::string> rules = ReadAllRules();
    for(int i=0; i<rules.size(); i++) {
        std::string rule = rules[i];
        if(Util::StringStartsWith(rule, "@reboot")) {
            std::vector<std::string> segments = Util::SplitString(rule, ' ');
            if(segments.size() >= 2) {
                if(segments[1] == "KiwiLight") {
                    validRules.push_back(rule);
                }
            }
        }
    }

    return validRules;
}
