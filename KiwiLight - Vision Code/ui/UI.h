#ifndef KiwiLight_UI_H
#define KiwiLight_UI_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "gtk-2.0/gtk/gtk.h"

/**
 * the main header for the UI module for KiwiLight
 * Written By: Brach Knutson
 */

using namespace std;

namespace KiwiLight {

    /**
     * Represents a generic UI widget.
     */
    class Widget {
        public:
        virtual GtkWidget *GetWidget() = 0;
    };

    /**
     * Represents a container. Used to organize UI objects inside a window.
     */
    class Panel : public Widget {
        public:
        Panel() {};
        Panel(bool horizontal, int spacing);
        void Pack_start(Widget &widget, bool expand, bool fill, int pad);
        void Pack_end(Widget &widget, bool expand, bool fill, int pad);
        GtkWidget *GetWidget() { return panel; };

        private:
        GtkWidget *panel;
    };


    class Frame : public Widget {
        public:
        Frame() {};
        Frame(string label);
        void Pack(Widget &widget);
        GtkWidget *GetWidget() { return this->frame; };

        private:
        GtkWidget *frame;
    };

    /**
     * Represents a UI window that contains a form, or what have you
     */
    class Window : public Widget {
        public:
        Window();
        void SetPane(Panel pane);
        void Show();
        void SetSize(int w, int h);
        int SetInterval(int interval, void(*method)());
        void RemoveInterval(int id);
        void Main() { gtk_main(); };
        GtkWidget *GetWidget() { return this->window; };
        
        private:
        GtkWidget *window;
        static void(*timeoutMethod)();
        static gboolean timeoutCallMethod();
    };

    /**
     * Represents a label with text. 
     */
    class Label : public Widget {
        public:
        Label() {};
        Label(string text);
        void SetText(string text);
        string GetText() { return text; };
        void SetFont(string font);
        void SetFontSize(double size);
        GtkWidget *GetWidget() { return this->label; };

        private:
        string text;
        string font;
        double font_size;
        GtkWidget *label;
    };

    /**
     * Represents a user-editable textbox.
     */
    class TextBox : public Widget {
        public:
        TextBox() {};
        TextBox(string text);
        void SetText(string text);
        void SetEditable(bool edit);
        void SetAlignment(double align);
        string GetText();
        GtkWidget *GetWidget() { return this->textbox; };

        private:
        GtkWidget *textbox;
    };

    /**
     * Represents a checkbox with boolean value
     */
    class CheckBox : public Widget {
        public:
        CheckBox() {};
        CheckBox(string name, bool checked);
        void SetState(bool checked);
        bool GetState();
        GtkWidget *GetWidget() { return this->checkbox; };

        private:
        GtkWidget *checkbox;
    };

    /**
     * Represents a user-editable scale widget.
     */
    class Slider : public Widget {
        public:
        Slider() {};
        Slider(string label, int min, int max, double step, double value);
        void SetValue(double value);
        double GetValue();
        GtkWidget *GetWidget() { return this->slider; };

        private:
        GtkWidget *slider;
    };

    /**
     * A faint line that can be placed between to objects in a container
     */
    class Separator : public Widget {
        public:
        Separator(bool horizontal);
        GtkWidget *GetWidget() { return this->separator; };

        private:
        GtkWidget *separator;
    };

    /**
     * Represents a button with a static method callback
     */
    class Button : public Widget {
        public:
        Button(string text, void(*callback)() );
        void SetText(string text);
        string GetText() { return text; };
        void SetCallback( void(*callback)() );
        GtkWidget *GetWidget() { return this->button; };

        private:
        string text;
        GtkWidget *button;
    };

    /**
     * A container that can align the objects inside of them.
     */
    class AlignmentContainer : public Widget {
        public:
        AlignmentContainer(int x, int y);
        void Pack(Widget &widget);
        GtkWidget *GetWidget() { return this->aligncontainer; };

        private:
        GtkWidget *aligncontainer;
    };

    /**
    * An option that would show under a menu bar item.
    */
    class SubMenuItem : public Widget {
        public:
        SubMenuItem(string name, void(*callback)());
        GtkWidget *GetWidget() { return this->submenuitem; };

        private:
        GtkWidget *submenuitem;
    };

    /**
     * A menu item able to be seen on a menu bar(ex "file", "help", etc)
     */
    class MenuItem : public Widget {
        public:
        MenuItem(string name);
        void AddSubmenuItem(SubMenuItem item);
        GtkWidget *GetWidget() { return this->menuitem; };

        private:
        GtkWidget *menuitem;
        GtkWidget *menu;
    };

    /**
     * A menu bar that would show up at the top of the screen.
     */
    class MenuBar : public Widget {
        public:
        MenuBar();
        void AddItem(MenuItem item);
        GtkWidget *GetWidget() { return this->menubar; };

        private:
        GtkWidget *menubar;
    };

    /**
     * A singlular widget adjusting a camera setting
     */
    class CameraSetting : public Widget {
        public:
        CameraSetting(string name, int min, int max, int value);
        int GetValue();
        string GetName() { return name; };
        GtkWidget *GetWidget() { return this->camerasetting; };

        private:
        GtkWidget *camerasetting;

        Label  nameLabel;
        TextBox input;

        string name,
               type;

        int min, 
            max,
            value;
    };
}

#endif