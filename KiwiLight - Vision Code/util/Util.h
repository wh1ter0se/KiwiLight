#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

namespace KiwiLight {
    
    /**
     * An easy event and variable flagging system
     */
    class Flags {
        public:
        static void RaiseFlag(string flagName);
        static void LowerFlag(string flagName);
        static void SetFlagState(string flagName, bool state);
        static bool GetFlag(string flagName);

        private:
        static vector<string> flagNames;
    };

    /**
     * Class that can execute shell commands and return their output.
     */
    class Shell {
        public:
        static string ExecuteCommand(string command);
    };

    class StringUtils {
        public:
        static vector<string> SplitString(string str, char character);
    };
}

#endif