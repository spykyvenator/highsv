#ifndef HIGHSVACTIONS_H
#define HIGHSVACTIONS_H

#include "highsvFile.h"

void quit_activated (GSimpleAction *action, GVariant *parameter, gpointer app);
void open_tab (GSimpleAction *action, GVariant *parameter, gpointer app);
void close_tab (GSimpleAction *action, GVariant *parameter, gpointer app);

#endif
