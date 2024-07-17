#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>


  // a_start_ has num_col_1 entries, and the last entry is the number
  // of nonzeros in A, allowing the number of nonzeros in the last
  // column to be defined
  // model.lp_.a_matrix_.start_ = {0, 2, 5};
  // model.lp_.a_matrix_.index_ = {1, 2, 0, 1, 2};
  // model.lp_.a_matrix_.value_ = {1.0, 3.0, 1.0, 2.0, 2.0};
typedef struct ll ll;

typedef struct ll {
    size_t size;
    char *val;
    ll *next;
} ll;

    // a_start_ has num_col_1 entries, and the last entry is the number
    // of nonzeros in A, allowing the number of nonzeros in the last
    // column to be defined
    // it is thus the index of each column start in A

    // a_index are the indices in our rows of values in a_value
    // The position in a_index/a_value of the index/value of the first nonzero in each vector is stored in a_start
    // a_start[0] = 0
typedef struct {
    int num_col, num_row, num_nz, sense;// num_nz is the number of nonzeros in a_value ( its length ) all are being set

    double col_cost[200], col_lower[200], col_upper[200], row_lower[200], row_upper[200], a_value[200], offset;// all are set

    int a_start[200], a_index[200], a_format;// TODO

    int nIndex[2];// set
    char *varNames[100];// set
} model_t;

typedef struct {
        double *col_value, *col_dual, *row_value, *row_dual, objective_value;
        int *col_basis_status, *row_basis_status,
            run_status, model_status;
} sol;

sol* solveModel(model_t *m);
void printSol(sol *solution, model_t *mod);

#endif
