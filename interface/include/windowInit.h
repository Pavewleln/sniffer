#ifndef WINDOW_INIT
#define WINDOW_INIT

struct GtkWidgets {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *combo_box;
    GtkWidget *listbox;
};

struct GtkWidgets* WindowInit();

void OnListRowActivated();

#endif