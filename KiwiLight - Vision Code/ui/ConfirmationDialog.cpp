#include "UI.h"

/**
 * Source file for the ConfirmationDialog class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new ConfirmationDialog displaing "message."
 */
ConfirmationDialog::ConfirmationDialog(std::string message) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->widget = gtk_dialog_new_with_buttons("Confirmation", GTK_WINDOW(window), GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_REJECT, NULL);
    GtkWidget *dialogContent = gtk_dialog_get_content_area(GTK_DIALOG(this->widget));
        this->content = Panel(false, 0);
            Label lbl = Label(message);
                this->content.Pack_start(lbl.GetWidget(), false, false, 0);

            gtk_container_add(GTK_CONTAINER(dialogContent), this->content.GetWidget());
    
}

/**
 * Sets the body of the ConfirmationDialog to the Panel "pnl." This is so that one can add features or prompts to the dialog.
 */
void ConfirmationDialog::SetBody(Panel pnl) {
    this->content.Pack_start(pnl.GetWidget(), true, true, 0);
}

/**
 * Shows the ConfirmationDialog but does not close it upon button press.
 */
bool ConfirmationDialog::ShowButDontClose() {
    gtk_widget_show_all(this->widget);
    gint response = gtk_dialog_run(GTK_DIALOG(this->widget));
    return response == GTK_RESPONSE_ACCEPT;
}

/**
 * Shows the ConfirmationDialog as a normal window, but does not run it as a dialog.
 */
void ConfirmationDialog::ShowWithoutRunning() {
    gtk_widget_show_all(this->widget);
}

/**
 * Shows the ConfirmationDialog normally, and returns it's response.
 */
bool ConfirmationDialog::ShowAndGetResponse() {
    gtk_widget_show_all(this->widget);
    gint response = gtk_dialog_run(GTK_DIALOG(this->widget));
    gtk_widget_destroy(this->widget);
    return response == GTK_RESPONSE_ACCEPT;
}

/**
 * Closes and destroys the ConfirmationDialog.
 */
void ConfirmationDialog::Destroy() {
    gtk_widget_destroy(this->widget);
}