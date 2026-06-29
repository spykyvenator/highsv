#ifndef HIGHSVACTIONS_H
#define HIGHSVACTIONS_H

#include "highsvFile.h"

void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
void open_tab(GSimpleAction *action, GVariant *parameter, gpointer app);
void open_new_tab(GSimpleAction *action, GVariant *parameter, gpointer app);
void close_tab(GSimpleAction *action, GVariant *parameter, gpointer app);
void save_tab(GSimpleAction *action, GVariant *parameter, gpointer app);
void save_tab_by_pointer(GtkButton *button, gpointer data);
void close_tab_by_pointer(GtkButton *button, gpointer notebook);
void close_errormsg(GtkButton *button, gpointer data);
void solve_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void range_tab(GSimpleAction *action, GVariant *parameter, gpointer app);
void toggleMip(GtkCheckButton *self, gpointer app);
void togglePos(GtkCheckButton *self, gpointer app);
void complete(GSimpleAction *action, GVariant *parameter, gpointer app);
void search(GSimpleAction *a, GVariant *parameter, gpointer app);
void search_changed_cb(GtkSearchEntry *entry, GtkTextBuffer *buffer);
void zoomIn(GSimpleAction *a, GVariant *parameter, gpointer app);
void showSaveBtn(GtkTextBuffer* buffer, GtkTextBufferNotifyFlags flags, guint position, guint length, gpointer user_data);
#endif
