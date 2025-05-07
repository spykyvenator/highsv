#include "highsvWin.h"
#include "highsvFile.h"
#include "highsvSol.h"

extern gboolean mip, pos;


void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  g_application_quit(G_APPLICATION(app));
}

void
open_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  openNew(NULL, win);
}

void
open_new_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  openNew(NULL, win);
}

void
close_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  closeActive(NULL, win);
}

void
close_tab_by_pointer(GtkButton *button, gpointer notebook)
{
  GtkWidget *box;

  box = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_BOX);
  gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), box);
}

void
save_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  saveActive(NULL, win);
}

void
solve_tab(GSimpleAction *action, GVariant *parameter, gpointer app)
{
  HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
  solveEntry(NULL, win);
}

/*
 * could I set the sol button action here instead?
 */
void
toggleMip(GtkCheckButton *self, gpointer app)
{
	mip = gtk_check_button_get_active(self);
}

void
togglePos(GtkCheckButton *self, gpointer app)
{
	pos = gtk_check_button_get_active(self);
}
