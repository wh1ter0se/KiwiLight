#include "UI.h"

/**
 * Source file for the LogViewer class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


LogViewer::LogViewer(std::string fileName) {

}


void LogViewer::Show() {

}


void LogViewer::SetName(std::string name) {
    gtk_widget_set_name(this->logviewer, name.c_str());
}