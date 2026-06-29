#ifndef HIGHSVFILE_H
#define HIGHSVFILE_H
void closeActive(GtkEntry *entry , HighsvAppWindow *win);
void openNew(GtkEntry *entry, HighsvAppWindow *win);
void openNewEmpty(GtkEntry *entry, HighsvAppWindow *win);
void openWithText(HighsvAppWindow *win, const char *text, const size_t len);
void saveActive(GtkEntry *entry, HighsvAppWindow *win);
void saveAsActive(GtkEntry *entry, HighsvAppWindow *win);
void saveFile(GFile *file, const char *content);
char* getContentFromTab(GtkWidget *tab);
void setViewContent(GtkWidget *view, GFile *file);
#endif

