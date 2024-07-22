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

void
saveActive(GtkEntry *entry, HighsvAppWindow *win)
{
    GtkWidget* tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    GtkWidget* view = gtk_widget_get_first_child(tab);
    
}

typedef struct {
  GtkFileDialog *fd;
  HighsvAppWindow *win;
} FileOpenData;

void
handleOpen(GObject* source_object, GAsyncResult* res, gpointer data)
{
  gboolean success = FALSE;
  FileOpenData *fopen = (FileOpenData *)data;

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
  FileOpenData *res = g_malloc(sizeof(FileOpenData));
  res->fd = gtk_file_dialog_new();
  res->win = win;
  gtk_file_dialog_open(res->fd, GTK_WINDOW(res->win), NULL, (handleOpen), res);
}

void
openWithText(HighsvAppWindow *win, const char *text, const size_t len)
{
    GError *error = NULL;
    GFileIOStream* stream = NULL;
    gsize bw;

    GFile *new = g_file_new_tmp("result-XXXXXX", &stream, &error);
    if (!new) {
        g_printerr("Error creating temp file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    GOutputStream* ostream = g_io_stream_get_output_stream(G_IO_STREAM(stream));
    if (!g_output_stream_write_all(ostream, text, len, &bw, NULL, &error)) {
        g_printerr("Error writing to file: %s\n", error->message);
        g_clear_error(&error);
    }

    if (!g_output_stream_close(ostream, NULL, &error)){
        g_printerr("Error closing out-stream: %s\n", error->message);
        g_clear_error(&error);
    }
    if (!g_io_stream_close((GIOStream*)stream, NULL, NULL)) {
        g_printerr("Error closing stream: %s\n", error->message);
        g_clear_error(&error);
    }

    highsv_app_window_open(win, new);
}
