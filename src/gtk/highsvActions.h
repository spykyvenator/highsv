#ifndef HIGHSVACTIONS_H
#define HIGHSVACTIONS_H

#include "highsvFile.h"

void quit_activated (GSimpleAction *action, GVariant *parameter, gpointer app);
void open_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void open_new_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void close_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void save_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void close_tab_by_pointer(GtkButton *button, gpointer notebook);
void solve_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void range_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void toggleMip(GtkCheckButton *self, gpointer app);
void togglePos(GtkCheckButton *self, gpointer app);
void complete (GSimpleAction *action, GVariant *parameter, gpointer app);
void zoomIn(GSimpleAction *a, GVariant *parameter, gpointer app);
#endif
