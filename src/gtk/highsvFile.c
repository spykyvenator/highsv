#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "highsv.h"
#include "highsvWin.h"

void
closeActive(GtkEntry *entry , HighsvAppWindow *win)
{
    GtkWidget* tab = getNotebookActive(GTK_NOTEBOOK(win->notebook));
    gtk_notebook_prev_page(GTK_NOTEBOOK(win->notebook));
    if (!tab) {
        return;
    }
    gtk_notebook_detach_tab(GTK_NOTEBOOK(win->notebook), tab);
}

typedef struct {
  GtkFileDialog *fd;
  HighsvAppWindow *win;
} FileData;

static void
handleOpen(GObject* source_object, GAsyncResult* res, gpointer data)
{
  GError *error = NULL;
  FileData *fopen = (FileData *)data;

  GFile *file = gtk_file_dialog_open_finish(fopen->fd, res, &error);
  if (error) {
      g_printerr("Error opening file: %s\n", error->message);
      g_clear_error(&error);
  }

  if (!file)
      return;

  highsv_app_window_open(fopen->win, file);
  // TODO reset button pos -> in notebook button possible?
  g_free(fopen);
}

void
openNew(GtkEntry *entry, HighsvAppWindow *win)
{
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_add_mime_type(filter, "text/plain");
  gtk_file_filter_set_name(filter, "text");

  GListStore *list = g_list_store_new(GTK_TYPE_FILE_FILTER);
  g_list_store_append(list, filter);

  FileData *res = g_malloc(sizeof(FileData));
  res->fd = gtk_file_dialog_new();
  res->win = win;

  gtk_file_dialog_set_filters(res->fd, G_LIST_MODEL(list));
  gtk_file_dialog_set_modal(res->fd, FALSE);
  gtk_file_dialog_open(res->fd, GTK_WINDOW(res->win), NULL, (handleOpen), res);
}

void
openNewEmpty(GtkEntry *entry, HighsvAppWindow *win)
{
  highsv_app_window_open_empty(win);
}

void
saveFile(GFile *file, const char *content)
{
  GError *error = NULL;
  GFileIOStream *stream;
  gsize bw;

  if (g_file_query_exists(file, NULL))
    stream = g_file_open_readwrite(file, NULL, &error);
  else
    stream = g_file_create_readwrite(file, G_FILE_CREATE_NONE, NULL, &error);

  GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(stream));// what??
        
  if (!g_output_stream_write_all(ostream, content, strlen(content), &bw, NULL, &error)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }

  goffset pos = g_seekable_tell(G_SEEKABLE(ostream));

  if (!g_seekable_truncate(G_SEEKABLE(ostream), pos, NULL, &error)) {
      g_printerr("Error truncating file: %s\n", error->message);
      g_clear_error(&error);
    }

  if (!g_output_stream_close(ostream, NULL, &error)) {
      g_printerr("Error closing ostream: %s\n", error->message);
      g_clear_error(&error);
  }

  if (!g_io_stream_close((GIOStream*) stream, NULL, &error)) {
      g_printerr("Error closing stream: %s\n", error->message);
      g_clear_error(&error);
  }
}

char*
getContentFromTab(GtkWidget *tab)
{
  GtkTextBuffer *buffer;
  GtkTextIter startI, endI;

  GtkWidget *view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

  gtk_text_buffer_get_start_iter(buffer, &startI);
  gtk_text_buffer_get_end_iter(buffer, &endI);

  const char *content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);
  return content;
}

static void
handleSave(GObject* source_object, GAsyncResult* res, gpointer data)
{
  GtkWidget *tab;

  gboolean success = FALSE;
  FileData *fsave = (FileData *)data;

  GFile *file = gtk_file_dialog_save_finish(fsave->fd, res, NULL);

  if (!file)
      return;

  tab = getNotebookActive(GTK_NOTEBOOK(fsave->win->notebook));

  char *content = getContentFromTab(tab);


  saveFile(file, content);

  g_object_unref(file);
  g_free(fsave);
  g_free(content);
} 

void
saveActive(GtkEntry *entry, HighsvAppWindow *win)
{
  GtkWidget *tab = getNotebookActive(win->notebook);
  gchar *path = g_object_get_data(G_OBJECT(tab), "path");

  if (!path) {
      FileData *res = g_malloc(sizeof(FileData));
      res->fd = gtk_file_dialog_new();
      res->win = win;

      gtk_file_dialog_save(res->fd, GTK_WINDOW(res->win), NULL, (handleSave), res);
  } else {
      GFile *file = g_file_new_for_path(path);

      gtk_widget_set_visible(
              GTK_WIDGET(g_object_get_data(G_OBJECT(tab), "saveBtn")), 
              false);
      saveFile(file, getContentFromTab(tab));

      g_object_unref(file);
  }
}

static void
handleFileLoad(GObject *src, GAsyncResult *res, gpointer data)
{
    GtkTextBuffer *buffer;
    gsize length;
    GtkWidget *view = GTK_WIDGET(data);
    GtkWidget *scrolled = gtk_widget_get_parent(view);
    GtkWidget *notebook = g_object_get_data(G_OBJECT(scrolled), "parent_notebook");
    GFile *file = G_FILE(src);
    char *contents;
    GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
    GError *error = NULL;

    g_file_load_contents_finish(file, res, &contents, &length, NULL, &error);
    if (!error) {
        gtk_source_language_manager_prepend_search_path(manager, "./src/gtk/");
        GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, "highsv");

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), lang);
        gtk_text_buffer_set_text(buffer, contents, length);

        gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), scrolled, getTabLabel(scrolled, file, gtk_text_view_get_buffer(GTK_TEXT_VIEW(view))));

        g_free(contents);
    } else {
        g_printerr("Error opening file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
} 

void
setViewContent(GtkWidget *view, GFile *file)
{
    g_file_load_contents_async(file, NULL, handleFileLoad, view);
}
