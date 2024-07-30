#include "highsvWin.h"
#include "highsvFile.h"


void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer app)
{
  g_application_quit(G_APPLICATION(app));
}

void
open_tab (GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  openNew(NULL, win);
}

void
close_tab (GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  closeActive(NULL, win);
}

void
save_tab (GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  saveActive(NULL, win);
}

