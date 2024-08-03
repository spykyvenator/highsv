#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"
#include "highsvActions.h"

struct _HighsvApp
{
  GtkApplication parent;
};

G_DEFINE_TYPE(HighsvApp, highsv_app, GTK_TYPE_APPLICATION);

static void
highsv_app_init (HighsvApp *app)
{
}


static GActionEntry app_entries[] =
{
  { "quit", quit_activated, NULL, NULL, NULL },
  { "open", open_tab, NULL, NULL, NULL },
  { "close", close_tab, NULL, NULL, NULL },
  { "save", save_tab, NULL, NULL, NULL }
};

static void
highsv_app_startup(GApplication *app)
{
  const char *quit_accels[2] = { "<ctrl>q", NULL };
  const char *open_accels[2] = { "<ctrl>o", NULL };
  const char *close_accels[2] = { "<ctrl>w", NULL };
  const char *save_accels[2] = { "<ctrl>s", NULL };

  G_APPLICATION_CLASS(highsv_app_parent_class)->startup (app);

  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", quit_accels); 
  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.open", open_accels); 
  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.close", close_accels); 
  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.save", save_accels); 
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

  windows = gtk_application_get_windows(GTK_APPLICATION(app));
  if (windows)
    win = HIGHSV_APP_WINDOW(windows->data);
  else
    win = highsv_app_window_new(HIGHSV_APP (app));

  for (i = 0; i < n_files; i++)
    highsv_app_window_open(win, files[i]);

  if (n_files == 0) {
    GFileIOStream *tmpio = NULL;
    GFile *tmp = g_file_new_tmp(NULL, &tmpio, NULL);
    highsv_app_window_open(win, tmp);
  }

  gtk_window_present(GTK_WINDOW (win));
}

void 
highsv_shutdown(GApplication *app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    g_object_unref(win->stack);// ? how to remove warning
}

static void
highsv_app_class_init (HighsvAppClass *class)
{
  G_APPLICATION_CLASS(class)->activate = highsv_app_activate;
  G_APPLICATION_CLASS(class)->open = highsv_app_open;
  G_APPLICATION_CLASS(class)->startup = highsv_app_startup;
  //G_APPLICATION_CLASS(class)->shutdown = highsv_shutdown;
}

HighsvApp *
highsv_app_new (void)
{
  return g_object_new(HIGHSV_APP_TYPE, "application-id", "org.gtk.highsvapp", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
