#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"

void
closeActive(GtkEntry *entry , HighsvAppWindow *win)
{
    GtkWidget* tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    gtk_stack_remove(GTK_STACK(win->stack), tab);
    /* TODO: select another window upon closing
    GtkSelectionModel *s = gtk_stack_get_pages(GTK_STACK(win->stack));
    GtkBitset *selected, *mask;
    selected = gtk_bitset_new_range(1, 1);
    mask = gtk_bitset_new_range(3, 1);
    gtk_selection_model_set_selection(s, selected, mask);
    g_free(selected);
    g_free(mask);
    g_free(s);
    const char *name = gtk_stack_get_visible_child_name(GTK_STACK(win->stack));
    if (name)
      puts(name);
    else
      puts("NULL");
    gtk_stack_set_visible_child(GTK_STACK(win->stack), scrolled);
    */
}

typedef struct {
  GtkFileDialog *fd;
  HighsvAppWindow *win;
} FileData;

static void
handleOpen(GObject* source_object, GAsyncResult* res, gpointer data)
{
  gboolean success = FALSE;
  FileData *fopen = (FileData *)data;

  GFile *file = gtk_file_dialog_open_finish(fopen->fd, res, NULL);

  if (!file)
      return;

  highsv_app_window_open(fopen->win, file);
  // TODO reset button pos -> in stack button possible?
  g_free(fopen);
} 

void
openNew(GtkEntry *entry, HighsvAppWindow *win)
{
  FileData *res = g_malloc(sizeof(FileData));
  res->fd = gtk_file_dialog_new();
  res->win = win;
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
  g_output_stream_close(ostream);
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
