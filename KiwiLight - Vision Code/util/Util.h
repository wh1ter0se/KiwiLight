#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"


using namespace cv;

namespace KiwiLight {
    
    /**
     * An easy event and variable flagging system
     */
    class Flags {
        public:
        static void RaiseFlag(std::string flagName);
        static void LowerFlag(std::string flagName);
        static void SetFlagState(std::string flagName, bool state);
        static bool GetFlag(std::string flagName);

        private:
        static std::vector<std::string> flagNames;
    };

    /**
     * Class that can execute shell commands and return their output.
     */
    class Shell {
        public:
        static std::string ExecuteCommand(std::string command);
    };

    class StringUtils {
        public:
        static std::vector<std::string> SplitString(std::string str, char character);
    };
}

#endif