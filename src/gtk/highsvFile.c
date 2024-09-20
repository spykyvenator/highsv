#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"

GtkWidget* 
get_previous_stack_child(GtkStack *stack, GtkWidget *current_child) {
    GtkSelectionModel *pages = gtk_stack_get_pages(GTK_STACK(stack));

    if (pages == NULL || current_child == NULL) {
        return NULL; // No pages or no visible child in the stack
    }

    GListModel *list_model = G_LIST_MODEL(pages);
    guint n_pages = g_list_model_get_n_items(list_model);

    for (guint i = 0; i < n_pages; i++) {
        GtkStackPage *page = GTK_STACK_PAGE(g_list_model_get_item(list_model, i));
        if (gtk_stack_page_get_child(page) == current_child) {
            gint prev_index = (i - 1 + n_pages) % n_pages;
            GtkStackPage *prev_page = GTK_STACK_PAGE(g_list_model_get_item(list_model, prev_index));
            return gtk_stack_page_get_child(prev_page);
        }
    }

    return NULL; // Should not reach here if current_child is in the stack
}

void
closeActive(GtkEntry *entry , HighsvAppWindow *win)
{
    GtkWidget* tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    GtkWidget* new = get_previous_stack_child(GTK_STACK(win->stack), tab);
    if (!new || !tab) {
        return;
    }
    gtk_stack_remove(GTK_STACK(win->stack), tab);
    gtk_stack_set_visible_child(GTK_STACK(win->stack), GTK_WIDGET(new));
}

typedef struct {
  GtkFileDialog *fd;
  HighsvAppWindow *win;
} FileData;

static void
handleOpen(GObject* source_object, GAsyncResult* res, gpointer data)
{
  GError *error = NULL;
  gboolean success = FALSE;
  FileData *fopen = (FileData *)data;

  GFile *file = gtk_file_dialog_open_finish(fopen->fd, res, &error);
  if (error) {
      g_printerr("Error opening file: %s\n", error->message);
      g_clear_error(&error);
  }

  if (!file)
      return;

  highsv_app_window_open(fopen->win, file);
  // TODO reset button pos -> in stack button possible?
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

static void
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

  if (!g_output_stream_close(ostream, NULL, &error)) {
      g_printerr("Error closing ostream: %s\n", error->message);
      g_clear_error(&error);
  }

  if (!g_io_stream_close((GIOStream*) stream, NULL, &error)) {
      g_printerr("Error closing stream: %s\n", error->message);
      g_clear_error(&error);
  }
}

static void
handleSave(GObject* source_object, GAsyncResult* res, gpointer data)
{
  char* content;
  GtkWidget *tab;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  GtkTextIter startI, endI;

  gboolean success = FALSE;
  FileData *fsave = (FileData *)data;

  GFile *file = gtk_file_dialog_save_finish(fsave->fd, res, NULL);

  if (!file)
      return;

  tab = gtk_stack_get_visible_child(GTK_STACK(fsave->win->stack));
  view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

  gtk_text_buffer_get_start_iter(buffer, &startI);
  gtk_text_buffer_get_end_iter(buffer, &endI);

  content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);

  saveFile(file, content);

  g_free(fsave);
  g_free(content);
} 

void
saveActive(GtkEntry *entry, HighsvAppWindow *win)
{
  FileData *res = g_malloc(sizeof(FileData));
  res->fd = gtk_file_dialog_new();
  res->win = win;

  gtk_file_dialog_save(res->fd, GTK_WINDOW(res->win), NULL, (handleSave), res);
}
