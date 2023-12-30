#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/windowInit.h"

void OnListRowActivated(GtkListBox *listbox, GtkListBoxRow *row, gpointer user_data) {
    gint index = gtk_list_box_row_get_index(row);
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
    const gchar *text = gtk_label_get_text(GTK_LABEL(label));

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Выбран пакет:\n%s\nИндекс: %d", text, index);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

struct GtkWidgets* WindowInit() {
    struct GtkWidgets* widgets = malloc(sizeof(struct GtkWidgets));

    widgets->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "Анализатор трафика");
    gtk_container_set_border_width(GTK_CONTAINER(widgets->window), 10);
    gtk_widget_set_size_request(widgets->window, 700, 500);
    g_signal_connect(widgets->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    widgets->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(widgets->window), widgets->vbox);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
    gtk_container_add(GTK_CONTAINER(widgets->vbox), scrolled_window);
    gtk_widget_set_hexpand(scrolled_window, TRUE);

    widgets->listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), widgets->listbox);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1,450);

    g_signal_connect(widgets->listbox, "row-activated", G_CALLBACK(OnListRowActivated), NULL);

    gtk_widget_show_all(widgets->window);

    return widgets;
}