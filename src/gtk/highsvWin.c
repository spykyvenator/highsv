#include <gtk/gtk.h>

#include "highsv.h"
#include "highsvWin.h"
#include "../parse/parse.h"
#include "../sol.h"

struct _HighsvAppWindow
{
  GtkApplicationWindow parent;

  GtkWidget *stack;
};

static void
rangeAnalysis(GtkEntry *entry, HighsvAppWindow *win)
{
    char* content;
    GtkWidget *tab;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    GtkTextIter startI, endI;
    
    tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    gtk_text_buffer_get_start_iter(buffer, &startI);
    gtk_text_buffer_get_end_iter(buffer, &endI);

    content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);
    printf("content: %s\n", content);
}

static void
solveModel(GtkEntry *entry, HighsvAppWindow *win)
{
    char* content;
    GtkWidget *tab;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    GtkTextIter startI, endI;
    
    tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    gtk_text_buffer_get_start_iter(buffer, &startI);
    gtk_text_buffer_get_end_iter(buffer, &endI);

    content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);

    parseString(content);

    GFileIOStream* stream = NULL;
    GFile *new = g_file_new_tmp("result", &stream, NULL);
    GOutputStream* ostream = g_io_stream_get_output_stream((GIOStream*) stream);
    gsize bw;
    g_output_stream_write_all(ostream, "testing\0", 8, &bw, NULL, NULL);
    g_output_stream_close(ostream, NULL, NULL);
    g_io_stream_close((GIOStream*)stream, NULL, NULL);

    highsv_app_window_open(win, new);
    //printf("content: %s\n", content);
}

static void
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
} FileOpenData;

static void
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

static void
openNew(GtkEntry *entry, HighsvAppWindow *win)
{
  FileOpenData *res = g_malloc(sizeof(FileOpenData));
  res->fd = gtk_file_dialog_new();
  res->win = win;
  gtk_file_dialog_open(res->fd, GTK_WINDOW(res->win), NULL, (handleOpen), res);
}


G_DEFINE_TYPE(HighsvAppWindow, highsv_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void
highsv_app_window_init (HighsvAppWindow *win)
{
  gtk_widget_init_template (GTK_WIDGET (win));
}

static void
highsv_app_window_class_init(HighsvAppWindowClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/org/highsvapp/window.ui");
  gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), HighsvAppWindow, stack);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), solveModel);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), rangeAnalysis);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), openNew);
  gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), closeActive);
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
  gtk_widget_set_hexpand (scrolled, TRUE);
  gtk_widget_set_vexpand (scrolled, TRUE);
  view = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW(view), TRUE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(view), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
  gtk_stack_add_titled(GTK_STACK(win->stack), scrolled, basename, basename);

  gtk_stack_set_visible_child(GTK_STACK(win->stack), scrolled);

  if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
    {
      GtkTextBuffer *buffer;

      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
      gtk_text_buffer_set_text (buffer, contents, length);
      g_free (contents);
    }

  g_free (basename);
}
