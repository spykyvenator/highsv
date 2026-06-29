#ifndef HIGHSVTAB_H
#define HIGHSVTAB_H
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "highsvWin.h"
#include "highsvActions.h"

void openTabSearchDialog(GtkWidget *tab);
void highsv_app_window_open(HighsvAppWindow *win, GFile *file);
void highsv_app_window_open_empty(HighsvAppWindow *win);
void setTabLabel(GtkNotebook *notebook, GtkWidget *scrolled, GFile *file);
GtkWidget* getTabLabel(GtkWidget *t, GFile *file, GtkTextBuffer *b);

static inline GtkWidget*
tabToScrolled(GtkWidget* tab)
{
    GtkWidget *overlay = gtk_search_bar_get_child(GTK_SEARCH_BAR(tab));
    GtkWidget *scrolled = gtk_overlay_get_child(GTK_OVERLAY(overlay));
    return  scrolled;
}
#endif
