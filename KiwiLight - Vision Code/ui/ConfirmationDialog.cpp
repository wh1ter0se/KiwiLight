#include "UI.h"

/**
 * Source file for the ConfirmationDialog class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


ConfirmationDialog::ConfirmationDialog(std::string message) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->dialog = gtk_dialog_new_with_buttons("Confirmation", GTK_WINDOW(window), GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_REJECT, NULL);
    GtkWidget *dialogContent = gtk_dialog_get_content_area(GTK_DIALOG(this->dialog));
        GtkWidget *label = gtk_label_new(message.c_str());
        gtk_container_add(GTK_CONTAINER(dialogContent), label);
    
}


bool ConfirmationDialog::ShowAndGetResponse() {
    gtk_widget_show_all(this->dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(this->dialog));
    gtk_widget_destroy(this->dialog);
    return response == GTK_RESPONSE_ACCEPT;
}


void ConfirmationDialog::SetName(std::string name) {
    gtk_widget_set_name(this->dialog, name.c_str());
}