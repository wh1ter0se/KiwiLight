#ifndef KiwiLight_H
#define KiwiLight_H

#include <iostream>
#include <string>
#include <fstream>
#include "ui/UI.h"
#include "util/Util.h"
#include "runner/Runner.h"

/**
 * the main header for the KiwiLight program and namespace
 * Written by: Brach Knutson
 */


using namespace cv;

namespace KiwiLight {

    enum UIMode {
        UI_STREAM,
        UI_RUNNER,
        UI_EDITOR,
        UI_CONFIG_RUNNING,
        UI_QUTTING
    };

}

#endif