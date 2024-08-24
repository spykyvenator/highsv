#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>
#include <gtk/gtk.h>

sol* solveModel(model_t *m);
void printSol(sol *solution, model_t *mod);
void printSolToFile(sol *solution, model_t *mod, GOutputStream* ostream);

#endif
