#include "Runner.h"

/**
 * Source file for the TroubleshootingWindow class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


TroubleshootingWindow::TroubleshootingWindow(TroubleshootingData data[], int dataLen) {
    this->window = Window(GTK_WINDOW_TOPLEVEL);
        Panel content = Panel(false, 0);
            Label title = Label("Troubleshoot Results");
                title.SetName("header");
                content.Pack_start(title.GetWidget(), false, false, 0);

            for(int i=0; i<dataLen; i++) {
                Label header = Label("Contour " + std::to_string(i));
                    header.SetName("subHeader");
                    content.Pack_start(header.GetWidget(), false, false, 0);

                Label horizontalDistanceLabel = Label("Horizontal Distance: ");
                //TODO finish this
            }

            this->window.SetContentPane(content);
        
        this->troubleshootingwindow = this->window.GetWidget();
}


void TroubleshootingWindow::Show() {
    this->window.Show();
}


void TroubleshootingWindow::SetName(std::string name) {
    gtk_widget_set_name(this->troubleshootingwindow, name.c_str());
}