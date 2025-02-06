#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"
#include "highsvFile.h"
#include "highsvSol.h"
#include "highsvActions.h"
//#include "../parse/parse.h"

G_DEFINE_TYPE(HighsvAppWindow, highsv_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
highsv_app_window_init (HighsvAppWindow *win)
{
  gtk_widget_init_template (GTK_WIDGET (win));
}

static void
highsv_app_window_class_init(HighsvAppWindowClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/org/highsvapp/window.ui");

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), HighsvAppWindow, stack);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), solveEntry);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), rangeAnalysis);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNew);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), closeActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), toggleMip);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), togglePos);
}

HighsvAppWindow *
highsv_app_window_new(HighsvApp *app)
{
  HighsvAppWindow *win = g_object_new(HIGHSV_APP_WINDOW_TYPE, "application", app, NULL);
  gtk_stack_set_hhomogeneous(GTK_STACK(win->stack), FALSE);
  return win;
}

void
highsv_app_window_open (HighsvAppWindow *win, GFile *file)
{
  char *basename;
  GtkWidget *scrolled, *view;
  char *contents;
  gsize length;

  basename = g_file_get_basename (file);

  scrolled = gtk_scrolled_window_new ();
  gtk_widget_set_hexpand(scrolled, TRUE);
  gtk_widget_set_vexpand(scrolled, TRUE);
  view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), TRUE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
  gtk_stack_add_titled(GTK_STACK(win->stack), scrolled, basename, basename);

  gtk_stack_set_visible_child(GTK_STACK(win->stack), scrolled);

  if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
    {
      GtkTextBuffer *buffer;

      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
      gtk_text_buffer_set_text(buffer, contents, length);
      g_free(contents);
    }

  g_free(basename);
}
