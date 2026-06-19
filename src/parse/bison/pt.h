#ifndef SM_H
#define SM_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HIGHS_BACKEND
#include "../../highs_interface.h"
#endif

typedef struct {
        double *vals;
        int *indices;
        double offset;
        size_t rL, rI, numNz;
} sm;

sm* init_sm(void);
sm* setVal(void *mod, sm *a, const char *var, const double val);
sm* mergeSm(sm *a, sm *b);
void apply_sm(sm *a, void *model);
void destroy_sm(sm *a);
void print_sm(sm *a);
void setObjective(void *mod, sm *a);
#endif
