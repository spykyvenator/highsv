#ifndef SOL_H
#define SOL_H
#include <gtk/gtk.h>
int parseString(const char *s, GOutputStream* ostream, gboolean mip, gboolean pos);
int parseStdin();
enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
int initModel();
int quitModel();
int parseFile(FILE *fd, GOutputStream* ostream, char mip, char pos);
#endif
