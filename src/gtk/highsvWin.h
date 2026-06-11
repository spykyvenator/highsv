#pragma once

#include <gtk/gtk.h>
#include "highsv.h"

struct _HighsvAppWindow
{
  GtkApplicationWindow parent;

  GtkNotebook *notebook;
};

#define HIGHSV_APP_WINDOW_TYPE (highsv_app_window_get_type ())
G_DECLARE_FINAL_TYPE (HighsvAppWindow, highsv_app_window, HIGHSV, APP_WINDOW, GtkApplicationWindow)


HighsvAppWindow *highsv_app_window_new(HighsvApp *app);
void highsv_app_window_open(HighsvAppWindow *win, GFile *file);
void highsv_app_window_open_empty(HighsvAppWindow *win);

GtkWidget* getTabLabel(GtkWidget *t, GFile *file, GtkTextBuffer *b);
inline static GtkWidget* 
getNotebookActive(GtkNotebook *n) 
{ 
    return gtk_notebook_get_nth_page(n, gtk_notebook_get_current_page(n));
}
