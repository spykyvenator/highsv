#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"

struct _HighsvApp
{
  GtkApplication parent;
};

G_DEFINE_TYPE(HighsvApp, highsv_app, GTK_TYPE_APPLICATION);

static void
highsv_app_init (HighsvApp *app)
{
}

static void
highsv_app_activate (GApplication *app)
{
  HighsvAppWindow *win;

  win = highsv_app_window_new (HIGHSV_APP (app));
  gtk_window_present (GTK_WINDOW (win));
}

static void
highsv_app_open (GApplication *app, GFile **files, int n_files, const char *hint)
{
  GList *windows;
  HighsvAppWindow *win;
  int i;

  windows = gtk_application_get_windows (GTK_APPLICATION (app));
  if (windows)
    win = HIGHSV_APP_WINDOW (windows->data);
  else
    win = highsv_app_window_new (HIGHSV_APP (app));

  for (i = 0; i < n_files; i++)
    highsv_app_window_open (win, files[i]);

  if (n_files == 0) {
    GFileIOStream *tmpio = NULL;
    GFile *tmp = g_file_new_tmp(NULL, &tmpio, NULL);
    highsv_app_window_open(win, tmp);
  }

  gtk_window_present(GTK_WINDOW (win));
}

static void
highsv_app_class_init (HighsvAppClass *class)
{
  G_APPLICATION_CLASS (class)->activate = highsv_app_activate;
  G_APPLICATION_CLASS (class)->open = highsv_app_open;
}

HighsvApp *
highsv_app_new (void)
{
  return g_object_new (HIGHSV_APP_TYPE, "application-id", "org.gtk.highsvapp", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
