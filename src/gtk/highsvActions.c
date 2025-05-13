#include <gtksourceview/gtksource.h>

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
  struct closeTab *temp = (struct closeTab*) notebook;
  gtk_notebook_detach_tab(temp->n, temp->t);
  g_free(temp);
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

void
complete(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *tab = getNotebookActive(GTK_NOTEBOOK(win->stack));
    GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));// owned by instance
    GtkSourceCompletion *c = gtk_source_view_get_completion(GTK_SOURCE_VIEW(view));
    gtk_source_completion_show(c);
}
