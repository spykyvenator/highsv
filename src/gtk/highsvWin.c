#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

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
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNew);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNewEmpty);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), closeActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), saveActive);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), toggleMip);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), togglePos);
}

HighsvAppWindow *
highsv_app_window_new(HighsvApp *app)
{
  HighsvAppWindow *win = g_object_new(HIGHSV_APP_WINDOW_TYPE, "application", app, NULL);
  return win;
}

void
highsv_app_window_open (HighsvAppWindow *win, GFile *file)
{
  char *basename;
  GtkWidget *scrolled, *view;
  char *contents;
  gsize length;
  GtkWidget *box, *label, *closeBtn;
  struct closeTab *temp = malloc(sizeof(struct closeTab));

  basename = g_file_get_basename (file);
  scrolled = gtk_scrolled_window_new ();
  view = gtk_source_view_new();
  label = gtk_label_new(basename);
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  closeBtn = gtk_button_new_with_label("X");

  gtk_widget_set_hexpand(scrolled, TRUE);
  gtk_widget_set_vexpand(scrolled, TRUE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), 1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), TRUE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), TRUE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 10);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(view), 10);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(view), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 10);
  gtk_text_view_set_input_hints(GTK_TEXT_VIEW(view), GTK_INPUT_HINT_SPELLCHECK);
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
  gtk_button_set_can_shrink(GTK_BUTTON(closeBtn), TRUE);
  gtk_button_set_has_frame(GTK_BUTTON(closeBtn), FALSE);
  temp->n = GTK_NOTEBOOK(win->stack);
  temp->t = scrolled;
  g_signal_connect(G_OBJECT(closeBtn), "clicked", G_CALLBACK(close_tab_by_pointer), temp);
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), closeBtn);

  gtk_notebook_append_page(GTK_NOTEBOOK(win->stack), scrolled, NULL);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(win->stack), scrolled, TRUE);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(win->stack), scrolled, box);

  if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
  {
      GtkTextBuffer *buffer;
      GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
      gtk_source_language_manager_prepend_search_path(manager, ".");
      GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, "highsv");

      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
      gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), lang);
      gtk_text_buffer_set_text(buffer, contents, length);
      g_free(contents);
  }

  g_free(basename);
}
