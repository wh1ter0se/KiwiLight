#include "UI.h"

/**
 * Source file for the ConfirmationDialog class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


ConfirmationDialog::ConfirmationDialog(std::string message) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->dialog = gtk_dialog_new_with_buttons("Confirmation", GTK_WINDOW(window), GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_REJECT, NULL);
}


bool ConfirmationDialog::ShowAndGetResponse() {
    gtk_dialog_run(GTK_DIALOG(this->dialog));
    gtk_widget_destroy(this->dialog);
    return false;
}


void ConfirmationDialog::SetName(std::string name) {
    gtk_widget_set_name(this->dialog, name.c_str());
}