#include "interfaces/highs_c_api.h"

#include "model.h"
#include <stdlib.h>
#include <stdio.h>

sol*
solveModel(model_t *m)
{
    sol *res = (sol*) malloc(sizeof(sol));

    res->col_value = (double*) malloc(sizeof(double) * m->num_col);
    res->col_dual = (double*) malloc(sizeof(double) * m->num_col);
    res->row_value = (double*) malloc(sizeof(double) * m->num_row);
    res->row_dual = (double*) malloc(sizeof(double) * m->num_row);

    res->col_basis_status = (int*) malloc(sizeof(int) * m->num_col);
    res->row_basis_status = (int*) malloc(sizeof(int) * m->num_row);

    int format = kHighsMatrixFormatColwise;
    res->run_status = Highs_lpCall(m->num_col, m->num_row, m->num_nz, 
           format,  m->sense, m->offset, m->col_cost, m->col_lower, m->col_upper, m->row_lower, m->row_upper,
           m->a_start, m->a_index, m->a_value,
           res->col_value, res->col_dual, res->row_value, res->row_dual,
           res->col_basis_status, res->row_basis_status,
           &(res->model_status));
    res->objective_value = m->offset;
    for (int i = 0; i < m->num_col; i++) res->objective_value += res->col_value[i]*m->col_cost[i];
    return res;
}

void
printSol(sol *solution, model_t *mod) {
    if (solution->run_status != kHighsStatusOk) {
        puts("run was not ok, please check your model");
        return;
    }

    if (solution->model_status == kHighsModelStatusOptimal) {
        puts("Global optimal solution found.");
        printf("Objective value:\t\t%lf\nTotal Variables:\t\t%d\nTotal Constraints:\t\t%d\n\nVariable\tValue\t\t\tReduced Cost\n",
                solution->objective_value,
                mod->num_col, mod->num_row);
        for (int i = 0; i < mod->num_col; i++) {// cols are varnames and their values reduced cost is col's dual
            printf("%s\t\t%lf\t\t%lf\n", mod->varNames[i], solution->col_value[i], solution->col_dual[i]);
        }

        puts("\nRow\t\tSlack Or Surplus\tDual Price");
        for (int i = 0; i < mod->num_row; i++) {// row is objective dual is shadow price
            printf("%d\t\t%lf\t\t%lf\n", i, solution->row_value[i], solution->row_dual[i]);
        }
    }
}
