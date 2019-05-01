#include "UI.h"

/**
 * Source file for the FileChooser class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


FileChooser::FileChooser(bool writing) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->filechooser = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      "Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL
                                      );

}


std::string FileChooser::Show() {
    gint result = gtk_dialog_run(GTK_DIALOG(this->filechooser));
    std::string file = "";
    if(result == GTK_RESPONSE_ACCEPT) {
        file =  gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(this->filechooser));
    }

    gtk_widget_destroy(this->filechooser);
    return file;
}

void FileChooser::SetName(std::string name) {
    gtk_widget_set_name(this->filechooser, name.c_str());
}