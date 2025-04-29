/*
 * range printing with Highs_getRanging
 */
#ifndef PRINTO_H
#define PRINTO_H

#ifdef CLI
#include <stdio.h>
#include <gio/gio.h>
#else
#include <gtk/gtk.h>
#endif

void printSolToFile(void *mod, GOutputStream* ostr, double time);
#endif
