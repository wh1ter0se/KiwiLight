#include "UI.h"

/**
 * Source file for the FileChooser class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new FileChooser.
 * @param writing Should be true if you intend this filechooser to choose a file to save to, false if choosing to read from.
 * @param defaultFileName Only used if writing is true, the default name of the file to save to.
 */
FileChooser::FileChooser(bool writing, std::string defaultFileName) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if(writing) {
        this->filechooser = gtk_file_chooser_dialog_new("Save File",
                                        GTK_WINDOW(window),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        "Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "Save",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL
                                        );

        // gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(this->filechooser), defaultFileName.c_str());
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(this->filechooser), defaultFileName.c_str());
    } else {
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

    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(this->filechooser), getenv("HOME"));
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