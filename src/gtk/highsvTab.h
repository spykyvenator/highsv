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
void highsvShowError(const char *msg, GtkWidget *view, GtkTextBuffer *bfr, int x, int y, int x2, int y2);
int freeEM(GtkWidget *msg);

inline static GtkWidget* 
getNotebookActive(GtkNotebook *n) 
{ 
    return gtk_notebook_get_nth_page(n, gtk_notebook_get_current_page(n));
}
#endif
