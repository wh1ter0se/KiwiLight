#include "UI.h"

/**
 * Source file for the Window class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer pointer) {
    return FALSE;
}

static void Destroy() {
    gtk_main_quit();
}

void(*Window::timeoutMethod)() = 0;

/**
 * Creates a new window.
 */
Window::Window() {
    this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(this->window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(this->window, "destroy", G_CALLBACK(Destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(this->window), 0);
}

/**
 * Sets the main content pane to the given panel. A 
 * content pane must be set for a window or else no
 * content will be shown.
 */
void Window::SetPane(Panel pane) {
    gtk_container_add(GTK_CONTAINER(this->window), pane.GetWidget());
}

/**
 * Set the size of the window to the width w and
 * height h.
 */
void Window::SetSize(int w, int h) {
    gtk_window_set_default_size(GTK_WINDOW(this->window), w, h);
}

/**
 * Run a method on a timed interval.
 */
int Window::SetInterval(int interval, void(*method)()) {
    timeoutMethod = method;
    return g_timeout_add(interval, GtkFunction(timeoutCallMethod), NULL);
}

/**
 * Cancel an existing interval by given id
 */
void Window::RemoveInterval(int id) {
    gtk_timeout_remove(id);
}

/**
 * Shows the window and all widgets within it.
 */
void Window::Show() {
    gtk_widget_show_all(this->window);
}


gboolean Window::timeoutCallMethod() {
    timeoutMethod();
    return TRUE;
}