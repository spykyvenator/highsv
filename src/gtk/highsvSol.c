#include "highsv.h"
#include "highsvWin.h"
#include "../parse/parse.h"
#include "../sol.h"

void
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

void
solveEntry(GtkEntry *entry, HighsvAppWindow *win)
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

