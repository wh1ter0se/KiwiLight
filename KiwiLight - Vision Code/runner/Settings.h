#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Settings file for runner. Defines general rules for runner to follow.
 * Written By: Brach Knutson
 */

namespace KiwiLight {
    class RunnerSettings {
        public:
        static const bool USE_CAMERA = false;

        static constexpr double AR_ERROR       = 0.1,
                                SOLIDITY_ERROR = 0.1;


    };
}

#endif