#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"
#include "highsvActions.h"
#include "../sol.h"

struct _HighsvApp
{
  GtkApplication parent;
};

G_DEFINE_TYPE(HighsvApp, highsv_app, GTK_TYPE_APPLICATION)

static GActionEntry app_entries[] =
{
  { "quit", quit_activated, NULL, NULL, NULL },
  { "open", open_tab, NULL, NULL, NULL },
  { "openNew", open_new_tab, NULL, NULL, NULL },
  { "close", close_tab, NULL, NULL, NULL },
  { "save", save_tab, NULL, NULL, NULL },
  { "solve", solve_tab, NULL, NULL, NULL },
  { "complete", complete, NULL, NULL, NULL },
  { "zoomi", zoomIn, NULL, NULL, NULL },
};

static void
highsv_app_startup(GApplication *app)
{
  initModel();
  const char *quit_accels[2] = { "<ctrl>q", NULL };
  const char *open_accels[2] = { "<ctrl>o", NULL };
  const char *open_new_accels[2] = { "<ctrl>n", NULL };
  const char *close_accels[2] = { "<ctrl>w", NULL };
  const char *save_accels[2] = { "<ctrl>s", NULL };
  const char *solve_accels[3] = { "<ctrl>r", "<ctrl>Return", NULL };
  const char *complete_accels[3] = { "<ctrl>k", NULL };
  const char *zoomIn_accels[3] = { "<ctrl>p", NULL };

  G_APPLICATION_CLASS(highsv_app_parent_class)->startup (app);

  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
#define QUOTE(X) #X
#define EXPAND_AND_QUOTE(X) QUOTE(X)
#define SET_ACCELS(NAME) gtk_application_set_accels_for_action(GTK_APPLICATION(app), EXPAND_AND_QUOTE(app.NAME)  , NAME ## _accels);
  SET_ACCELS(quit);
  SET_ACCELS(open);
  SET_ACCELS(open_new);
  SET_ACCELS(close);
  SET_ACCELS(save);
  SET_ACCELS(solve);
  SET_ACCELS(zoomIn);
}
#undef SET_ACCELS

static void
highsv_app_add_notebookBtn(HighsvAppWindow *win)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *button = gtk_button_new_with_label("+");
  GtkWidget *openBtn = gtk_button_new_with_label("O");
  gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
  gtk_button_set_has_frame(GTK_BUTTON(openBtn), FALSE);
  gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Create new file");
  gtk_widget_set_tooltip_text(GTK_WIDGET(openBtn), "Open file");
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(openNewEmpty), win);
  g_signal_connect(G_OBJECT(openBtn), "clicked", G_CALLBACK(openNew), win);
  gtk_box_append(GTK_BOX(box), openBtn);
  gtk_box_append(GTK_BOX(box), button);
  gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_START);
  gtk_notebook_set_action_widget(GTK_NOTEBOOK(win->notebook), box, GTK_PACK_END);
}

static void
open_empty(gpointer data)
{
    HighsvAppWindow *win = (HighsvAppWindow*) data;
    while (!gtk_window_get_focus(GTK_WINDOW(win))) g_usleep(100);// check if the window is presented
    highsv_app_window_open_empty(win);
}

static void
highsv_app_activate(GApplication *app)
{
  HighsvAppWindow *win;
  GList *windows;
  int i;

  windows = gtk_application_get_windows(GTK_APPLICATION(app));
  if (windows)
    win = HIGHSV_APP_WINDOW(windows->data);
  else
    win = highsv_app_window_new(HIGHSV_APP(app));
  highsv_app_add_notebookBtn(win);

  gtk_window_present(GTK_WINDOW(win));
  g_idle_add_once(open_empty, win);
}

typedef struct {
   HighsvAppWindow *win; 
   int n_files;
   GFile **f;
} Files;

static void
open_files(gpointer data)
{
    Files *f = (Files*) data;
    while (!gtk_window_get_focus(GTK_WINDOW(f->win))) g_usleep(100);// check if the window is presented
    for (int i = 0; i < f->n_files; i++)
      highsv_app_window_open(f->win, f->f[i]);
    g_free(f->f);
    free(f);
}

static void
highsv_app_open(GApplication *app, GFile **files, int n_files, const char *hint)
{
  GList *windows;
  HighsvAppWindow *win;
  int i;

  windows = gtk_application_get_windows(GTK_APPLICATION(app));
  if (windows)
    win = HIGHSV_APP_WINDOW(windows->data);
  else
    win = highsv_app_window_new(HIGHSV_APP(app));

  highsv_app_add_notebookBtn(win);
  gtk_window_present(GTK_WINDOW(win));

  Files *f = (Files*) malloc(sizeof(Files));
  f->win = win;
  f->n_files = n_files;
  f->f = g_malloc(sizeof(GFile*) *n_files);
  for (int i = 0; i < n_files; i++)
      f->f[i] = g_object_ref(files[i]);

  g_idle_add_once(open_files, f);
}

static void 
highsv_app_shutdown(GApplication *app)
{
    quitModel();
    HighsvApp *highsv = (HighsvApp *) app;

  G_APPLICATION_CLASS (highsv_app_parent_class)
    ->shutdown (app);

    GList *windows;
    HighsvAppWindow *win;
    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows){
        win = HIGHSV_APP_WINDOW(windows->data);
        gtk_window_destroy(GTK_WINDOW(win));
    }
}

static void
highsv_app_init (HighsvApp *app)
{
}

static void
highsv_app_class_init (HighsvAppClass *class)
{
#define SET_FUNC(NAME) G_APPLICATION_CLASS(class)->NAME = highsv_app_ ## NAME;
    SET_FUNC(activate);
    SET_FUNC(open);
    SET_FUNC(startup);
    SET_FUNC(shutdown);
#undef SET_FUNC 
}

HighsvApp *
highsv_app_new(void)
{
  return g_object_new(HIGHSV_APP_TYPE, "application-id", "org.gtk.highsvapp", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
