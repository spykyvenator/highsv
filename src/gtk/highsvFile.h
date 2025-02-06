#ifndef HIGHSVFILE_H
#define HIGHSVFILE_H
void closeActive(GtkEntry *entry , HighsvAppWindow *win);
void handleOpen(GObject* source_object, GAsyncResult* res, gpointer data);
void openNew(GtkEntry *entry, HighsvAppWindow *win);
void openNewEmpty(GtkEntry *entry, HighsvAppWindow *win);
void openWithText(HighsvAppWindow *win, const char *text, const size_t len);
void saveActive(GtkEntry *entry, HighsvAppWindow *win);
#endif

