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

    /**
     * Panel where configuration can be viewed
     */
    class ConfigPanel : public Widget {
        public:
        ConfigPanel() {};
        ConfigPanel(std::string configFilePath);
        void LoadConfig(std::string fileName);
        void Clear();
        std::string GetConfig() { return this->configFile; };
        GtkWidget *GetWidget() { return this->configPanel; };
        void SetName(std::string name);

        private:
        Panel panel;
        Panel informationPanel;
        Panel buttonPanel;
        Label header,
              fileLabel,
              PreProcessorLabel,
              PostProcessorLabel,
              TargetLabel,
              UDPAddressLabel,
              UDPPortLabel;
        Button editConfig;
        std::string configFile;
        GtkWidget *configPanel;
    };


    enum UIMode {
        UI_STREAM,
        UI_RUNNER,
        UI_EDITOR,
        UI_CONFIG_RUNNING,
        UI_QUTTING
    };

}

#endif