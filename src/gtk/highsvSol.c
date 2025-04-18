#include "highsv.h"
#include "highsvWin.h"
#include "highsvFile.h"
#include "../sol.h"

extern gboolean mip, pos;

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
    g_free(content);
}

void
solveEntry(GtkEntry *entry, HighsvAppWindow *win)
{
    char* content, *newName;
    GtkWidget *tab;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    GtkTextIter startI, endI;
    size_t nameLen;
    
    tab = gtk_stack_get_visible_child(GTK_STACK(win->stack));
    const char *name = gtk_stack_get_visible_child_name(GTK_STACK(win->stack));
    view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(tab));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    gtk_text_buffer_get_start_iter(buffer, &startI);
    gtk_text_buffer_get_end_iter(buffer, &endI);

    content = gtk_text_buffer_get_text(buffer, &startI, &endI, TRUE);


    GError *error = NULL;
    GFileIOStream* stream = NULL;

    nameLen = strlen(name)+8;
    newName = malloc(sizeof(char)*nameLen);
    snprintf(newName, nameLen, "%s-XXXXXX", name);
    GFile *new = g_file_new_tmp(newName, &stream, &error);
    if (!new) {
        g_printerr("Error creating temp file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    GOutputStream* ostream = g_io_stream_get_output_stream(G_IO_STREAM(stream));

    parseString(content, ostream, mip, pos);

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
    if (!g_output_stream_close(ostream, NULL, &error)) {
        g_printerr("Error closing ostream: %s\n", error->message);
        g_clear_error(&error);
    }

    if (!g_io_stream_close((GIOStream*) stream, NULL, &error)) {
        g_printerr("Error closing stream: %s\n", error->message);
        g_clear_error(&error);
    }

    g_free(content);
}
