#ifndef SOL_H
#define SOL_H

#ifdef CLI
#include <stdio.h>
#include "./cli/cli.h"
#include <gio/gio.h>
#else
#include <gtk/gtk.h>
#endif

int parseString(const char *s, GOutputStream* ostream, gboolean mip, gboolean pos);
int parseStdin();
enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
int initModel();
int quitModel();
int parseFile(FILE *fd, GOutputStream* ostream, char mip, char pos);
#endif
