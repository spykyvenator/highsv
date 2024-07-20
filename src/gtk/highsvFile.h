#ifndef HIGHSVFILE_H
#define HIGHSVFILE_H
void closeActive(GtkEntry *entry , HighsvAppWindow *win);
void handleOpen(GObject* source_object, GAsyncResult* res, gpointer data);
void openNew(GtkEntry *entry, HighsvAppWindow *win);
#endif

