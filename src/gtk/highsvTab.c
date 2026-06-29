//TODO: migrate tab functions from win to tab
#include "highsvTab.h"
#include "highsvActions.h"

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
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(res), TRUE);
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
getSearchBar()
{
    GtkWidget *res = gtk_search_bar_new();
    gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(res), TRUE);
    gtk_search_bar_set_show_close_button(GTK_SEARCH_BAR(res), TRUE);
    gtk_widget_set_valign(res, GTK_ALIGN_END);
    gtk_widget_set_halign(res, GTK_ALIGN_END);

    return res;
}

void
setTabLabel(GtkNotebook *notebook, GtkWidget *overlay, GFile *file)
{
  char *basename;
  if (file)
      basename = g_file_get_basename (file);
  else
      basename = "unsaved";

  GtkWidget *box = gtk_notebook_get_tab_label(notebook, overlay);
  GtkWidget *label = gtk_widget_get_first_child(box);

  gtk_label_set_label(GTK_LABEL(label), basename);

  if (file) g_free(basename);
}

void
openTabSearchDialog(GtkWidget *tab)
{
    GtkWidget *entry, *searchBar, *view;
    GtkTextBuffer *buffer;

    searchBar = g_object_get_data(G_OBJECT(tab), "searchBar");
    if (searchBar) {
        entry = gtk_search_bar_get_child(GTK_SEARCH_BAR(searchBar));
        gtk_widget_set_visible(searchBar, TRUE);
        gtk_widget_grab_focus(entry);
        return;
    }

    view = tabToScrolled(tab);
    view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(view));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    searchBar = getSearchBar();
    entry = gtk_search_entry_new();

    gtk_search_bar_set_child(GTK_SEARCH_BAR(searchBar), entry);
    gtk_overlay_add_overlay(GTK_OVERLAY(tab), searchBar);
    gtk_widget_grab_focus(entry);
    g_object_set_data(G_OBJECT(tab), "searchBar", searchBar);
    g_signal_connect (entry, "search-changed",
                        G_CALLBACK(search_changed_cb), buffer);
}

/*
 * file is NULLable
 */
GtkWidget*
getTabLabel(GtkWidget *t, GFile *file, GtkTextBuffer *b)
{
  GtkWidget *box, *label, *closeBtn, *saveBtn;
  char *basename;
  if (file)
      basename = g_file_get_basename (file);
  else
      basename = "unsaved";

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
  if (file)
      g_free(basename);

  return box;
}

static GtkWidget*
makeTab(HighsvAppWindow *win)
{
    GtkWidget *scrolled, *view, *overlay;

    overlay = gtk_overlay_new();
    scrolled = getScrolledWin();
    view = getSourceView();

    g_object_set_data(G_OBJECT(scrolled), "parent_notebook", win->notebook);

    gtk_overlay_set_child(GTK_OVERLAY(overlay), scrolled);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
    gtk_scrolled_window_set_policy(
    GTK_SCROLLED_WINDOW(scrolled),
    GTK_POLICY_AUTOMATIC,
    GTK_POLICY_AUTOMATIC
    );
    setSourceCompletion(GTK_SOURCE_VIEW(view));
    int index = gtk_notebook_append_page(GTK_NOTEBOOK(win->notebook), overlay, NULL);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(win->notebook), index);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(win->notebook), overlay, TRUE);
    return scrolled;
}

void
highsv_app_window_open_empty(HighsvAppWindow *win)
{
  GtkWidget *scrolled = makeTab(win);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scrolled))));
  GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();

  gtk_source_language_manager_prepend_search_path(manager, "./src/gtk/");
  GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, "highsv");

  gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), lang);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(win->notebook), scrolled, getTabLabel(scrolled, NULL, buffer));
}

void
highsv_app_window_open(HighsvAppWindow *win, GFile *file)
{
  GtkWidget *scrolled = makeTab(win);

  g_object_set_data_full(G_OBJECT(scrolled), "path", g_file_get_path(file), (GDestroyNotify) g_free);

  setViewContent(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scrolled)), file);

}
