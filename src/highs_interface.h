#ifndef HIGHS_INTERFACE_H
#define HIGHS_INTERFACE_H


/*
    an interface between highsv and highs, 
    if I'd want to implement another backend, only this file should be changed
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
    return Highs_getColByName(mod, text, &index);
}

static inline int64_t
highsv_getNumCol(const void *model)
{
    return Highs_getNumCol(model);
}

static inline void
highsv_addVar(const void *model)
{
    Highs_addVar(mod, -Highs_getInfinity(mod), Highs_getInfinity(mod));
}

static inline void
highsv_passColName(const void* model, const int64_t index, const char *name)
{
    Highs_passColName(model, index, name);
}
#endif
