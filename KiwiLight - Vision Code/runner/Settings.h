#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>

/**
 * Settings file for runner. Defines general rules for runner to follow. Used for debugging
 * Written By: Brach Knutson
 */

namespace KiwiLight {
    /**
     * Defines debugging settings for the Runner to use.
     * For a release, USE_CAMERA should be true.
     */
    class RunnerSettings {
        public:
        static const bool USE_CAMERA;
        static const std::string IMAGE_TO_USE;
    };
}

#endif
