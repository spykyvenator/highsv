#include "highsv.h"
#include "highsvWin.h"
#include "highsvFile.h"
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
    char* content, *resBuffer;
    GtkWidget *tab;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    GtkTextIter startI, endI;
    size_t resLen;
    
    tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    gtk_text_buffer_get_start_iter(buffer, &startI);
    gtk_text_buffer_get_end_iter(buffer, &endI);

    content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);


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

    parseString(content, ostream);

    if (!g_output_stream_close(ostream, NULL, &error)){
        g_printerr("Error closing out-stream: %s\n", error->message);
        g_clear_error(&error);
    }
    if (!g_io_stream_close((GIOStream*)stream, NULL, NULL)) {
        g_printerr("Error closing stream: %s\n", error->message);
        g_clear_error(&error);
    }

    highsv_app_window_open(win, new);
    
    //printf("content: %s\n", content);
}
