#include "UI.h"

/**
 * Source file for the Window class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

void(*Window::onAppClosed)() = 0;
void(*Window::timeoutMethod)() = 0;

/**
 * Called when the window is deleted.
 */
gboolean Window::delete_event(GtkWidget *widget, GdkEvent *event, gpointer pointer) {
    Window::onAppClosed();
    return FALSE;
}

/**
 * Called when the window is destroyed, if the Window is created with terminateOnClose being true.
 */
void Window::Destroy() {
    gtk_main_quit();
}

/**
 * Sets what happens when the app is closed.
 */
void Window::SetOnAppClosed(void(*onAppClosed)()) {
    Window::onAppClosed = onAppClosed;
}

/**
 * Sets what happens when the window is destroyed.
 */
void Window::SetOnWindowClosed(void(*onWindowClosed)()) {
    g_signal_connect(this->widget, "delete-event", G_CALLBACK(onWindowClosed), NULL);
    g_signal_connect(this->widget, "destroy", G_CALLBACK(onWindowClosed), NULL);
}

/**
 * Creates a new window.
 * @param type The type of window.
 */
Window::Window(GtkWindowType type) {
    this->widget = gtk_window_new(type);
    g_signal_connect(this->widget, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(this->widget, "destroy", G_CALLBACK(Destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(this->widget), 0);
}

/**
 * Creates a new window.
 * @param type The type of window.
 * @param terminateOnClose True if the application should close when the Window closes, false otherwise.
 */
Window::Window(GtkWindowType type, bool terminateOnClose) {
    this->widget = gtk_window_new(type);
    gtk_container_set_border_width(GTK_CONTAINER(this->widget), 0);

    if(terminateOnClose) {
        g_signal_connect(this->widget, "delete-event", G_CALLBACK(delete_event), NULL);
        g_signal_connect(this->widget, "destroy", G_CALLBACK(Destroy), NULL);
    }
}

/**
 * Sets the main content pane to the given panel. A 
 * content pane must be set for a window or else no
 * content will be shown.
 */
void Window::SetPane(Panel pane) {
    gtk_container_add(GTK_CONTAINER(this->widget), pane.GetWidget());
}

/**
 * Set the size of the window to the width w and
 * height h.
 */
void Window::SetSize(int w, int h) {
    gtk_window_set_default_size(GTK_WINDOW(this->widget), w, h);
}

/**
 * Run a method on a timed interval.
 */
int Window::SetInterval(int interval, void(*method)()) {
    timeoutMethod = method;
    return g_timeout_add(interval, GSourceFunc(timeoutCallMethod), NULL);
}

/**
 * Cancel an existing interval by given id
 */
void Window::RemoveInterval(int id) {
    g_source_remove(id);
}

/**
 * Shows the window and all widgets within it.
 */
void Window::Show() {
    gtk_widget_show_all(this->widget);
}

/**
 * Sets the CSS stylesheet that this Window uses.
 */
void Window::SetCSS(std::string fileName) {
    GtkCssProvider *css = gtk_css_provider_new();
    GdkDisplay *disp = gdk_display_get_default();
    GdkScreen *screen = gdk_screen_get_default();
    gtk_css_provider_load_from_path(css, fileName.c_str(), NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); 
}

/**
 * Called if an interval is specified. Calls timeoutMethod().
 */
gboolean Window::timeoutCallMethod() {
    timeoutMethod();
    return TRUE;
}
