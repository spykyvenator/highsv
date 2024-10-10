#ifndef SOL_H
#define SOL_H
#include <gtk/gtk.h>
int parseString(const char *s, GOutputStream* ostream);
int parseStdin();
enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
#endif
