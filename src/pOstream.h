/*
 * range printing with Highs_getRanging
 */
#ifndef PRINTO_H
#define PRINTO_H

#ifdef CLI
#include <stdio.h>
#else
#include <gtk/gtk.h>
#endif

void printSolToFile(void *mod, GOutputStream* ostr);
#endif
