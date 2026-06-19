#ifndef HIGHS_INTERFACE_H
#define HIGHS_INTERFACE_H


/*
    an interface between highsv and highs, 
    if I'd want to implement another lp backend, only this file should be changed
*/

#include <stdint.h>
#include "util.h"
#include "interfaces/highs_c_api.h"
#include <stdlib.h>

#define HIGHSV_STATUS_ERROR 	                kHighsStatusError
#define HIGHSV_STATUS_NOTSET 	                kHighsModelStatusNotset
#define HIGHSV_STATUS_LOADERROR 	        kHighsModelStatusLoadError
#define HIGHSV_STATUS_MODELERROR 	        kHighsModelStatusModelError
#define HIGHSV_STATUS_PRESOLVEERROR 	        kHighsModelStatusPresolveError
#define HIGHSV_STATUS_SOLVEERROR 	        kHighsModelStatusSolveError
#define HIGHSV_STATUS_POSTSOLVEERROR 	        kHighsModelStatusPostsolveError
#define HIGHSV_STATUS_TIMELIMIT 	        kHighsModelStatusTimeLimit
#define HIGHSV_STATUS_ITERATIONLIMIT 	        kHighsModelStatusIterationLimit
#define HIGHSV_STATUS_UNKNOWN 	                kHighsModelStatusUnknown
#define HIGHSV_STATUS_SOLUTIONLIMIT 	        kHighsModelStatusSolutionLimit
#define HIGHSV_STATUS_INTERRUPT 	        kHighsModelStatusInterrupt
#define HIGHSV_STATUS_MODELEMPTY 	        kHighsModelStatusModelEmpty
#define HIGHSV_STATUS_OPTIMAL 	                kHighsModelStatusOptimal
#define HIGHSV_STATUS_INFEASIBLE 	        kHighsModelStatusInfeasible
#define HIGHSV_STATUS_UNBOUNDEDORINFEASIBLE 	kHighsModelStatusUnboundedOrInfeasible
#define HIGHSV_STATUS_UNBOUNDED 	        kHighsModelStatusUnbounded
#define HIGHSV_MAX_STRING_LENGTH 	        kHighsMaximumStringLength

#define HIGHSV_T_INT kHighsVarTypeInteger

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
highsv_getObjectiveOffset(const void *model)
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

/*
static inline void
highsv_getRowsByRange(const void* highs, const int64_t from_row,
                              const int64_t to_row, int64_t* num_row,
                              double* lower, double* upper, int64_t* num_nz,
                              int64_t* matrix_start, int64_t* matrix_index,
                              double* matrix_value)
{
    int64_t l = to_row - from_row + 1;
    int64_t cpnz = *num_nz;
    HighsInt nr;
    HighsInt ms[*num_nz], mi[*num_nz], nz;
    if (Highs_getRowsByRange(highs, (HighsInt) from_row,
                              (HighsInt) to_row, &nr,
                              lower, upper, &nz,
                              ms, mi,
                              matrix_value))
        die("could not get rows by range");
    if (num_row) *num_row = (int64_t) nr;
    if (num_nz) *num_nz = (int64_t) nz;

    for (int64_t i = 0; i < cpnz; i++) {
        if (matrix_start) matrix_start[i] = (int64_t) ms[i];
        if (matrix_index) matrix_index[i] = (int64_t) mi[i];
    }
}
*/

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

static inline int64_t
highsv_getNumNz(const void *highs)
{
    return Highs_getNumNz(highs);
}

static inline void
highsv_getSolution(const void *model, 
        double *col_val, double *col_dual, 
        double *row_val, double *row_dual)
{
    if (Highs_getSolution(model, col_val, col_dual, row_val, row_dual))
        die("could not get solution");
}

static inline double
highsv_getInfinity(const void *model)
{
    return Highs_getInfinity(model);
}

static inline void
highsv_getBasisInverseRow(const void* highs, const int64_t row,
                                  double* row_vector, int64_t* row_num_nz,
                                  int64_t* row_index)
{
        if (Highs_getBasisInverseRow(highs, (const HighsInt) row,
                                  row_vector, (HighsInt*) row_num_nz,
                                  (HighsInt*) row_index))
            die("could not get inverse row");
}

static inline void
highsv_getRanging(
    void* highs,
    double* col_cost_up_value, double* col_cost_up_objective,
    int64_t* col_cost_up_in_var, int64_t* col_cost_up_ou_var,
    double* col_cost_dn_value, double* col_cost_dn_objective,
    int64_t* col_cost_dn_in_var, int64_t* col_cost_dn_ou_var,
    double* col_bound_up_value, double* col_bound_up_objective,
    int64_t* col_bound_up_in_var, int64_t* col_bound_up_ou_var,
    double* col_bound_dn_value, double* col_bound_dn_objective,
    int64_t* col_bound_dn_in_var, int64_t* col_bound_dn_ou_var,
    double* row_bound_up_value, double* row_bound_up_objective,
    int64_t* row_bound_up_in_var, int64_t* row_bound_up_ou_var,
    double* row_bound_dn_value, double* row_bound_dn_objective,
    int64_t* row_bound_dn_in_var, int64_t* row_bound_dn_ou_var)
{
    if (Highs_getRanging(
        highs,
         col_cost_up_value,  col_cost_up_objective,
        (HighsInt*) col_cost_up_in_var, (HighsInt*) col_cost_up_ou_var,
         col_cost_dn_value,  col_cost_dn_objective,
        (HighsInt*) col_cost_dn_in_var, (HighsInt*) col_cost_dn_ou_var,
         col_bound_up_value,  col_bound_up_objective,
        (HighsInt*) col_bound_up_in_var, (HighsInt*) col_bound_up_ou_var,
         col_bound_dn_value,  col_bound_dn_objective,
        (HighsInt*) col_bound_dn_in_var, (HighsInt*) col_bound_dn_ou_var,
         row_bound_up_value,  row_bound_up_objective,
        (HighsInt*) row_bound_up_in_var, (HighsInt*) row_bound_up_ou_var,
         row_bound_dn_value,  row_bound_dn_objective,
        (HighsInt*) row_bound_dn_in_var, (HighsInt*) row_bound_dn_ou_var))
        die("could not get ranging");
}

static inline void 
highsv_getColName(const void* highs, const int64_t col, char* name)
{
    if (Highs_getColName(highs, (const HighsInt) col, name))
        die("failed to get col %d name", col);
}

static inline double 
highsv_getObjectiveValue(const void* highs)
{
    return Highs_getObjectiveValue(highs);
}

static inline void 
highsv_getRowName(const void* highs, const HighsInt row, char* name)
{
    if (Highs_getRowName(highs, (const HighsInt) row, name))
        die("could not get row name %d", row);
}

static inline int64_t
highsv_getModelStatus(const void* highs)
{
    return (int64_t) Highs_getModelStatus(highs);
}

static inline int 
highsv_versionMajor(void)
{
    return (int) Highs_versionMajor();
}

static inline int 
highsv_versionMinor(void)
{
        return (int) Highs_versionMinor();
}

static inline int 
highsv_versionPatch(void)
{
    return (int) Highs_versionPatch();
}

static inline void
highsv_destroy(void* highs)
{
    Highs_destroy(highs);
}

static inline void* 
highsv_create(void)
{
    return Highs_create();
}


static inline void
highsv_setBoolOptionValue(void* highs, const char* option, const int64_t value)
{
    if (Highs_setBoolOptionValue(highs, option,
                              (const HighsInt) value))
        die("could not set bool option %s %d", option, value);
}


static inline void 
highsv_changeColBounds(void* highs, const int64_t col,
                               const double lower, const double upper)
{
    if (Highs_changeColBounds(highs, (const HighsInt) col,
                                   lower, upper))
        die("could not change col %d bounds to: %f - %f", col, lower, upper);
}


static inline void 
highsv_changeColIntegrality(void* highs, const int64_t col,
                                    const int64_t integrality)
{
    if (Highs_changeColIntegrality(highs, (const HighsInt) col,
                                (const HighsInt) integrality))
        die("could not change col %d integrality to %d", col, integrality);
}
static inline void 
highsv_changeColsIntegralityByRange(void* highs,
                                            const int64_t from_col,
                                            const int64_t to_col,
                                            const int64_t* integrality)
{
    if (Highs_changeColsIntegralityByRange(highs,
                                            (const HighsInt) from_col,
                                            (const HighsInt) to_col,
                                            (const HighsInt*) integrality))
        die("could not change col %d to %d integrality", from_col, to_col);
}

static inline void
highsv_changeColsBoundsByRange(void* highs, const int64_t from_col,
                                       const int64_t to_col,
                                       const double* lower,
                                       const double* upper)
{
    if (Highs_changeColsBoundsByRange(highs, (const HighsInt) from_col,
                                   (const HighsInt) to_col, lower, upper))
        die("could not change col %d to %d range to %d to %d", from_col, to_col, lower, upper);
}
static inline void 
highsv_presolve(void* highs)
{
    if (Highs_presolve(highs))
        die("could not presolve");
}

static inline void 
highsv_run(void* highs)
{
    if (Highs_run(highs))
        die("could not run");
}
static inline int64_t 
highsv_getPresolvedNumCol(const void* highs)
{
        return (int64_t) Highs_getPresolvedNumCol(highs);
}

static inline int64_t 
highsv_getPresolvedNumRow(const void* highs)
{
    return (int64_t) Highs_getPresolvedNumRow(highs);
}

static inline void
highsv_addRow(void* highs, const double lower, const double upper,
              const HighsInt num_new_nz, const HighsInt* index,
              const double* value)
{
    if (Highs_addRow(highs, lower, upper, num_new_nz, index, value))
        die("could not set row");
}

static inline void
highsv_writeModel(void *highs, const char *filename)
{
    if (Highs_writeModel(highs, filename))
        die("failed to write model to: %s\n", filename);
}

static inline void
highsv_passRowName(void *mod, size_t numRow, const char* rowName)
{
  if (Highs_passRowName(mod, (HighsInt) numRow, rowName)) {
      die("could not set row name: %d to %s", numRow, rowName);
  }
}

static inline void
highsv_changeColsCostByRange(void *mod, const int from, const int to, const double *vals) 
{
    HighsInt f = (HighsInt) from;
    HighsInt t = (HighsInt) to;
    if (Highs_changeColsCostByRange(mod, f, t, vals))
        die("failed to set objective");
}
#endif
