#include "UI.h"

/**
 * Source file for the RunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


RunnerEditor::RunnerEditor(Runner runner) {
    Panel editor = Panel(false, 0);
    this->runnereditor = editor.GetWidget();
}


void RunnerEditor::SetName(std::string name) {
    gtk_widget_set_name(this->runnereditor, name.c_str());
}