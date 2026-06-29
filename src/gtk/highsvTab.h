#ifndef HIGHSVTAB_H
#define HIGHSVTAB_H
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "highsvWin.h"

void openTabSearchDialog(GtkWidget *tab);
void highsv_app_window_open(HighsvAppWindow *win, GFile *file);
void highsv_app_window_open_empty(HighsvAppWindow *win);
void setTabLabel(GtkNotebook *notebook, GtkWidget *scrolled, GFile *file);
GtkWidget* getTabLabel(GtkWidget *t, GFile *file, GtkTextBuffer *b);

static inline GtkWidget*
tabToScrolled(GtkWidget* tab)
{
    GtkWidget *scrolled = gtk_overlay_get_child(GTK_OVERLAY(tab));
    return  scrolled;
}
#endif
