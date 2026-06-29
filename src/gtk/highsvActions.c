#include <gtksourceview/gtksource.h>

#include "highsvWin.h"
#include "highsvFile.h"
#include "highsvSol.h"
#include "../util.h"

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
close_tab_by_pointer(GtkButton *button, gpointer data)
{
  GtkWidget *t = (GtkWidget*) data;
  GtkNotebook *n = g_object_get_data(G_OBJECT(t), "parent_notebook");

  gtk_notebook_remove_page(n, gtk_notebook_page_num(n, t));
}

void
close_errormsg(GtkButton *button, gpointer data)
{
    GtkWidget *revealer, *overlay;

    revealer = (GtkWidget*) data;
    overlay = gtk_widget_get_parent(GTK_WIDGET(revealer));

    gtk_revealer_set_reveal_child(GTK_REVEALER(revealer), FALSE);
    gtk_overlay_remove_overlay(GTK_OVERLAY(overlay), revealer);

    if (freeEM(revealer))
        die("failed to free errormsg");
}

void
save_tab_by_pointer(GtkButton *button, gpointer data)
{
  GtkWidget *temp = (GtkWidget*) data;
  gchar *path = (gchar*) g_object_get_data(G_OBJECT(temp), "path");
  GFile *f = g_file_new_for_path(path);

  gtk_widget_set_visible(GTK_WIDGET(button), false);
  if (!f) return;// silently ignore if no filename is present
  saveFile(f, getContentFromTab(temp));
  g_object_unref(f);
}

void
showSaveBtn(GtkTextBuffer* buffer, GtkTextBufferNotifyFlags flags, guint position, guint length, gpointer data) 
{
    GtkWidget *t = data;
    gtk_widget_set_visible(t, true);
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
zoomIn(GSimpleAction *a, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *tab = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));// owned by instance
    puts("ok");
    puts(gtk_widget_get_css_name(GTK_WIDGET(view)));
}

void
complete(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    HighsvAppWindow *win = HIGHSV_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    GtkWidget *tab = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));// owned by instance
    GtkSourceCompletion *c = gtk_source_view_get_completion(GTK_SOURCE_VIEW(view));
    gtk_source_completion_show(c);
}
