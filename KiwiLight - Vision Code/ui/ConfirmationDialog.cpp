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
        this->content = Panel(false, 0);
            Label lbl = Label(message);
                this->content.Pack_start(lbl.GetWidget(), false, false, 0);

            gtk_container_add(GTK_CONTAINER(dialogContent), this->content.GetWidget());
    
}


void ConfirmationDialog::SetBody(Panel pnl) {
    this->content.Pack_start(pnl.GetWidget(), true, true, 0);
}


bool ConfirmationDialog::ShowButDontClose() {
    gtk_widget_show_all(this->dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(this->dialog));
    return response == GTK_RESPONSE_ACCEPT;
}


void ConfirmationDialog::ShowWithoutRunning() {
    gtk_widget_show_all(this->dialog);
}


bool ConfirmationDialog::ShowAndGetResponse() {
    gtk_widget_show_all(this->dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(this->dialog));
    gtk_widget_destroy(this->dialog);
    return response == GTK_RESPONSE_ACCEPT;
}


void ConfirmationDialog::Destroy() {
    gtk_widget_destroy(this->dialog);
}


void ConfirmationDialog::SetName(std::string name) {
    gtk_widget_set_name(this->dialog, name.c_str());
}