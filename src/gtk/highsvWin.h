#pragma once

#include <gtk/gtk.h>
#include "highsv.h"

struct _HighsvAppWindow
{
  GtkApplicationWindow parent;

  GtkWidget *stack;
};

#define HIGHSV_APP_WINDOW_TYPE (highsv_app_window_get_type ())
G_DECLARE_FINAL_TYPE (HighsvAppWindow, highsv_app_window, HIGHSV, APP_WINDOW, GtkApplicationWindow)


HighsvAppWindow *highsv_app_window_new(HighsvApp *app);
void highsv_app_window_open(HighsvAppWindow *win, GFile *file);


