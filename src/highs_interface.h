#ifndef HIGHS_INTERFACE_H
#define HIGHS_INTERFACE_H


/*
    an interface between highsv and highs, 
    if I'd want to implement another lp backend, only this file should be changed
*/

#include <stdint.h>
#include "util.h"
#include "interfaces/highs_c_api.h"

#define HIGHSV_STATUS_ERROR kHighsStatusError

static inline void
highsv_setSenseMax(void *model)
{
    if (Highs_changeObjectiveSense(model, kHighsObjSenseMaximize)) 
            die("changing sense to max failed");
}

static inline void
highsv_setSenseMin(void *model)
{
    if (Highs_changeObjectiveSense(model, kHighsObjSenseMinimize)) 
            die("changing sense to max failed");
}

static inline int64_t
highsv_getColByName(const void* model, const char* name, int64_t* col)
{
    return Highs_getColByName(model, name, (HighsInt*) col);
}

static inline int64_t
highsv_getNumCol(const void *model)
{
    return Highs_getNumCol(model);
}

static inline void
highsv_addVar(void *model)
{
    Highs_addVar(model, -Highs_getInfinity(model), Highs_getInfinity(model));
}

static inline void
highsv_passColName(const void* model, const int64_t index, const char *name)
{
    Highs_passColName(model, index, name);
}

static inline void
highsv_changeColCost(void *model, const size_t index, const double value)
{
    Highs_changeColCost(model, (HighsInt) index, value);
}

static inline double
highsv_getObjectiveOffset(void *model)
{
    double res;
    if (Highs_getObjectiveOffset(model, &res))
        die("could not get objective offset");
    return res;
}

static inline void
highsv_setObjectiveOffset(void *model, const double val)
{
    if (Highs_changeObjectiveOffset(model, val))
        die("could not set objective offset");
}

static inline void
highsv_getRowsByRange(const void* highs, const int64_t from_row,
                              const int64_t to_row, int64_t* num_row,
                              double* lower, double* upper, int64_t* num_nz,
                              int64_t* matrix_start, int64_t* matrix_index,
                              double* matrix_value)
{
    if (Highs_getRowsByRange(highs, (HighsInt) from_row,
                              (HighsInt) to_row, (HighsInt*) num_row,
                              lower, upper, (HighsInt*) num_nz,
                              (HighsInt*) matrix_start, (HighsInt*) matrix_index,
                              matrix_value))
        die("could not get rows by range");
}

static inline void
highsv_getColsByRange(const void* highs, const int64_t from_col,
                              const int64_t to_col, int64_t* num_col,
                              double* costs, double* lower, double* upper,
                              int64_t* num_nz, int64_t* matrix_start,
                              int64_t* matrix_index, double* matrix_value)
{
    if (Highs_getColsByRange(highs, (HighsInt) from_col,
                              (HighsInt) to_col, (HighsInt*) num_col,
                              costs, lower, upper,
                              (HighsInt*) num_nz, (HighsInt*) matrix_start,
                              (HighsInt*) matrix_index, matrix_value))
        die("could not get cols by range");
}

#endif
