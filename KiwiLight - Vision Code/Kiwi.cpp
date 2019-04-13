#include "KiwiLight.h"

using namespace cv;
using namespace KiwiLight;

/**
 * Source file for the Kiwi class, an event manager that keeps event methods in one place.
 * Written By: Brach Knutson
 */


/**
 * Opens a form for the user to create a new vision config.
 */
void Kiwi::AddConfig() {
    std::cout << "new config" << std::endl;
    std::cout.flush();
}

/**
 * Saves the config currently in the editor.
 */
void Kiwi::SaveConfig() {
    std::cout << "save config" << std::endl;
    std::cout.flush();
}

/**
 * Runs the selected config, or does nothing if nothing is selected.
 */
void Kiwi::RunSelected() {
    std::cout << "run selected" << std::endl;
    std::cout.flush();
}

/**
 * Sets up the config to start when the computer boots up
 */
void Kiwi::ConfStartConfigOnBoot() {
    std::cout << "conf start on boot" << std::endl;
    std::cout.flush();
}

/**
 * Compiles the config and leaves it there.
 */
void Kiwi::CompileConfig() {
    std::cout << "compile config" << std::endl;
    std::cout.flush();
}

/**
 * Opens a file menu to open a config.
 */
void Kiwi::OpenConfig() {
    std::cout << "open config" << std::endl;
    std::cout.flush();
}

/**
 * Edits the selected config, or does nothing if nothing is selected.
 */
void Kiwi::EditSelected() {
    std::cout << "edit selected" << std::endl;
    std::cout.flush();
}

/**
 * Shows the about menu and flexes about how Foximus Prime is a cool FRC team
 */
void Kiwi::ShowAbout() {
    std::cout << "show about menu" << std::endl;
    std::cout.flush();
}

/**
 * directs you to the github repo where tutorials for the app are shown
 */
void Kiwi::HELPME() {
    std::cout << "HELP" << std::endl;
    std::cout.flush();
}
