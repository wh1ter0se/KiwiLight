#include "KiwiLight.h"

/**
 * 3695 thing here
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Displays the KiwiLight help message.
 */
void ShowHelp() {
    std::cout << "KIWILIGHT HELP\n";
    std::cout << "Usage: KiwiLight [options] [config files]\n";
    std::cout << "\n";
    std::cout << "KiwiLight is a smart vision solution for FRC applications developed by FRC Team 3695: Foximus Prime.\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "-c: Runs a config file, or multiple config files.\n";
    std::cout << "-h: Displays this help message.\n";
    std::cout << std::endl;
}

/**
 * Runs the configurations specified in filePaths. A KiwiLight log will be produced.
 */
void RunConfigs(std::vector<std::string> filePaths) {
    std::cout << "run file: " << filePaths[0] << std::endl;

    //init needed KiwiLight variables
    KiwiLightApp::ReconnectUDP("127.0.0.1", 3695, false);

    std::cout << "Command: Run Configs\n";
    std::cout << "Config files found: " << filePaths.size() << "\n";
    std::cout << "Ensure that all config files share the same UDP Address and Port." << std::endl;
    
    //initalize the runners
    std::cout << "\nInitalizing Runners" << std::endl;
    const int numTargets = (const int) filePaths.size();
    Runner runners[numTargets];
    std::string 
        runnerNames = "",
        runnerFiles = "";
    int totalContours = 0;
    for(int i=0; i<filePaths.size(); i++) {
        runners[i] = Runner(filePaths[i], false);
        totalContours += runners[i].NumberOfContours();

        runnerNames += runners[i].GetConfName();
        runnerFiles += runners[i].GetFileName();
        if(i < filePaths.size() - 1) {
            runnerNames += ",";
            runnerFiles += ",";
        }
    }
    
    //wait for the KiwiLight sender to connect
    KiwiLightApp::WaitForSocket();

    //create a logger
    std::string logFileBase = "";
    char *home = getenv("HOME");
    if(home != NULL) {
        logFileBase = std::string(home) + "/KiwiLightData/logs/";
    } else {
        std::cout << "WARNING: The HOME Environment variable could not be found! The Log file will not be generated!" << std::endl;
    }
    std::string logFileName = logFileBase + "KiwiLight-Runner-Log-" + Clock::GetDateString() + ".xml";
    Logger logger = Logger(logFileName);
    logger.SetConfName(runnerNames, runnerFiles);
    logger.Start();

    //show the cool header in the terminal
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "                                            " << std::endl;
    std::cout << " Starting KiwiLight                         " << std::endl;
    std::cout << " Targets:     " << filePaths.size() << std::endl;
    std::cout << " Contours:    " << totalContours << std::endl;
    std::cout << " UDP Address: " << KiwiLightApp::GetUDP().GetAddress() << std::endl;
    std::cout << " UDP Port:    " << KiwiLightApp::GetUDP().GetPort() << std::endl;
    std::cout << "                                            " << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    while(KiwiLightApp::CurrentMode() == AppMode::UI_HEADLESS) {
        Target closestTarget;
        int closestTargetID;

        double 
            closestTargetHorizontalAngle = 360,
            closestTargetVerticalAngle = 360,
            closestTargetObliqueAngle = 360;
        
        bool targetFound = false;

        for(int i=0; i<numTargets; i++) {
            std::string result = runners[i].Iterate();
            if(result == Runner::NULL_MESSAGE) {
                continue;
            } else {
                targetFound = true;
            }

            Target currentClosestTarget = runners[i].GetClosestTargetToCenter();
            
            //calculate 3d angle to target
            Point robotCenter = runners[i].GetLastFrameCenterPoint();
            double currentHorizontalAngle = currentClosestTarget.HorizontalAngle(robotCenter.x);
            double currentVerticalAngle   = currentClosestTarget.VerticalAngle(robotCenter.y);
            double currentObliqueAngle    = currentClosestTarget.ObliqueAngle(robotCenter.x, robotCenter.y);

            if(currentObliqueAngle < closestTargetObliqueAngle) {
                closestTarget = currentClosestTarget;
                closestTargetID = i;
                closestTargetHorizontalAngle = currentHorizontalAngle;
                closestTargetVerticalAngle   = currentVerticalAngle;
                closestTargetObliqueAngle    = currentObliqueAngle;
            }
        }

        //formulate string based on closest target
        std::string message = Runner::NULL_MESSAGE;

        if(targetFound) {
            message = Util::composeRioMessage(
                closestTargetID,
                closestTarget.Center().x,
                closestTarget.Center().y,
                closestTarget.Bounds().width,
                closestTarget.Bounds().height,
                closestTarget.Distance(),
                closestTargetHorizontalAngle,
                closestTargetVerticalAngle
            );
        }
        
        KiwiLightApp::SendOverUDP(message);
        logger.Log(message);
    }
}

/**
 * Test method. This method will be run if the -t flag is specified.
 */
void Test() {
    std::cout << "No Tests to Perform. Put some in the Test() method!\n";
}

/**
 * Main entry point for KiwiLight!! This method will analyze the command args and decide what to do.
 */
int main(int argc, char *argv[]) {
    if(argc == 1) {
        KiwiLightApp::Create(argc, argv);
        KiwiLightApp::Start();
    } else {
        bool runningConfig = false;
        bool showHelp = false;
        std::vector<std::string> confsToRun;

        for(int i=0; i<argc; i++) {
            std::string argument = std::string(argv[i]);

            if(argument == "-t") {
                Test();
                return 0;
            }

            if(argument == "-c") {
                runningConfig = true;
            }

            if(argument == "-h") {
                ShowHelp();

                if(runningConfig) {
                    //"-c" option was already entered
                    std::cout << "WARNING: Options \"-c\" and \"-h\" should not be used together. Please use one or the other.\n";
                    return 0;
                }
            }

            if(argument.length() > 4) {
                if(argument.substr(argument.length() - 4) == ".xml") {
                    confsToRun.push_back(argument);
                }
            }
        }

        if(runningConfig) {
            RunConfigs(confsToRun);
        }

        if(!(runningConfig || showHelp)) {
            std::cout << "No valid command arguments found.\n";
            std::cout << "Use \"KiwiLight -h\" to see the command options, or just \"KiwiLight\" to launch the GUI!" << std::endl;
        }
    }
    
    return 0;
}
