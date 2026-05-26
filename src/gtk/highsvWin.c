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
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/org/highsvapp/window.ui");

  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), HighsvAppWindow, notebook);
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
    GtkSourceSnippet *s = gtk_source_snippet_new_parsed("max", NULL);

    gtk_source_completion_words_register(prv, gtk_text_view_get_buffer(GTK_TEXT_VIEW(v)));
    gtk_source_completion_add_provider(sc, GTK_SOURCE_COMPLETION_PROVIDER(prv));
    gtk_source_view_push_snippet(v, s, NULL);
    gtk_source_view_set_enable_snippets(v, TRUE);
}

static inline GtkWidget*
getSourceView()
{
  GtkWidget *res = gtk_source_view_new();
  GtkCssProvider *provider = gtk_css_provider_new ();
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(res), TRUE);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(res), TRUE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(res), TRUE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(res), 10);
  gtk_text_view_set_input_hints(GTK_TEXT_VIEW(res), GTK_INPUT_HINT_SPELLCHECK);
  gtk_css_provider_load_from_string(provider,// this probably needs to be set globally instead
     "textview { font-family: Monospace; font-size: 12pt; }");
  gtk_style_context_add_provider(gtk_widget_get_style_context(res),
      GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(res), TRUE);
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(res), TRUE);

  g_object_unref (provider);

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
getTabLabel(GtkWidget *t, GFile *file, GtkTextBuffer *b)
{
  GtkWidget *box, *label, *closeBtn, *saveBtn;
  char *basename = g_file_get_basename (file);

  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  label = gtk_label_new(basename);
  saveBtn = gtk_button_new_with_label("💾");
  closeBtn = gtk_button_new_with_label("✕");

  gtk_button_set_can_shrink(GTK_BUTTON(closeBtn), TRUE);
  gtk_button_set_has_frame(GTK_BUTTON(closeBtn), FALSE);
  gtk_widget_set_tooltip_text(closeBtn, "close tab");
  gtk_button_set_can_shrink(GTK_BUTTON(saveBtn), TRUE);
  gtk_button_set_has_frame(GTK_BUTTON(saveBtn), FALSE);
  gtk_widget_set_tooltip_text(saveBtn, "save tab");

  g_signal_connect(G_OBJECT(closeBtn), "clicked", 
          G_CALLBACK(close_tab_by_pointer), t);
  g_signal_connect(G_OBJECT(saveBtn), "clicked", 
          G_CALLBACK(save_tab_by_pointer), t);

  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), saveBtn);
  gtk_box_append(GTK_BOX(box), closeBtn);
  gtk_text_buffer_add_commit_notify(b, GTK_TEXT_BUFFER_NOTIFY_AFTER_INSERT | GTK_TEXT_BUFFER_NOTIFY_AFTER_DELETE, 
          showSaveBtn, 
          saveBtn, NULL);

  g_object_set_data(G_OBJECT(t), "saveBtn", saveBtn);


  gtk_widget_set_visible(saveBtn, false);
  g_free(basename);

  return box;
}

void
highsv_app_window_open(HighsvAppWindow *win, GFile *file)
{
  GtkWidget *scrolled, *view;
  char *contents;
  gsize length;
  GtkTextBuffer *buffer;

  scrolled = getScrolledWin();
  view = getSourceView();
  g_object_set_data_full(G_OBJECT(scrolled), "path", g_file_get_path(file), (GDestroyNotify) g_free);
  g_object_set_data(G_OBJECT(scrolled), "parent_notebook", win->notebook);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
  gtk_scrolled_window_set_policy(// TODO: check if this solves scrolling on opening issue -> it does not
    GTK_SCROLLED_WINDOW(scrolled),
    GTK_POLICY_AUTOMATIC,
    GTK_POLICY_AUTOMATIC
    );
  setSourceCompletion(GTK_SOURCE_VIEW(view));
  int index = gtk_notebook_append_page(GTK_NOTEBOOK(win->notebook), scrolled, NULL);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(win->notebook), scrolled, TRUE);

  if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
  {
      GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
      gtk_source_language_manager_prepend_search_path(manager, "./src/gtk/");
      GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, "highsv");

      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
      gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), lang);
      gtk_text_buffer_set_text(buffer, contents, length);
      gtk_notebook_set_tab_label(GTK_NOTEBOOK(win->notebook), scrolled, getTabLabel(scrolled, file, buffer));

      g_free(contents);
  }
  gtk_notebook_set_current_page(GTK_NOTEBOOK(win->notebook), index);
}
