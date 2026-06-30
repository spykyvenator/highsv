#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "highsv.h"
#include "highsvWin.h"
#include "highsvFile.h"
#include "highsvSol.h"
#include "highsvActions.h"
#include "../util.h"
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
  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/highsvapp/window.ui");

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), HighsvAppWindow, notebook);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), solveEntry);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNew);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNewEmpty);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), closeActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveAsActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), toggleMip);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), togglePos);
}

HighsvAppWindow *
highsv_app_window_new(HighsvApp *app)
{
  HighsvAppWindow *win = g_object_new(HIGHSV_APP_WINDOW_TYPE, "application", app, NULL);
  return win;
}
