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

static inline void
setSourceCompletion(GtkSourceView *v)
{
    GtkSourceCompletionWords *prv = gtk_source_completion_words_new(NULL);
    GtkSourceCompletion *sc = gtk_source_view_get_completion(v);
    gtk_source_completion_words_register(prv, gtk_text_view_get_buffer(GTK_TEXT_VIEW(v)));
    gtk_source_completion_add_provider(sc, GTK_SOURCE_COMPLETION_PROVIDER(prv));
}

static inline GtkWidget*
getSourceView()
{
  GtkWidget *res = gtk_source_view_new();
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(res), 1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(res), TRUE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(res), TRUE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_input_hints(GTK_TEXT_VIEW(res), GTK_INPUT_HINT_SPELLCHECK);
  gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(res), TRUE);
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(res), TRUE);

  return res;
}

static inline GtkWidget*
getScrolledWin()
{
  GtkWidget *res = gtk_scrolled_window_new ();
  gtk_widget_set_hexpand(res, TRUE);
  gtk_widget_set_vexpand(res, TRUE);

  return res;
}
static inline GtkWidget*
getTabLabel(GtkNotebook *n, GtkWidget *t, GFile *file)
{
  GtkWidget *box, *label, *closeBtn;
  struct closeTab *temp = malloc(sizeof(struct closeTab));
  char *basename = g_file_get_basename (file);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  label = gtk_label_new(basename);
  closeBtn = gtk_button_new_with_label("X");

  gtk_button_set_can_shrink(GTK_BUTTON(closeBtn), TRUE);
  gtk_button_set_has_frame(GTK_BUTTON(closeBtn), FALSE);
  temp->n = n;
  temp->t = t;
  g_signal_connect(G_OBJECT(closeBtn), "clicked", 
          G_CALLBACK(close_tab_by_pointer), temp);
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), closeBtn);

  g_free(basename);

  return box;
}

void
highsv_app_window_open (HighsvAppWindow *win, GFile *file)
{
  GtkWidget *scrolled, *view;
  char *contents;
  gsize length;

  scrolled = getScrolledWin();
  view = getSourceView();

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
  setSourceCompletion(GTK_SOURCE_VIEW(view));
  gtk_notebook_append_page(GTK_NOTEBOOK(win->stack), scrolled, NULL);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(win->stack), scrolled, TRUE);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(win->stack), scrolled, getTabLabel(GTK_NOTEBOOK(win->stack), scrolled, file));

  if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
  {
      GtkTextBuffer *buffer;
      GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
      gtk_source_language_manager_prepend_search_path(manager, "./src/gtk/");
      GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, "highsv");

      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
      gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), lang);
      gtk_text_buffer_set_text(buffer, contents, length);
      g_free(contents);
  }
}
