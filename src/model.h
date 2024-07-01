#ifndef MODEL_H
#define MODEL_H


  // a_start_ has num_col_1 entries, and the last entry is the number
  // of nonzeros in A, allowing the number of nonzeros in the last
  // column to be defined
  // model.lp_.a_matrix_.start_ = {0, 2, 5};
  // model.lp_.a_matrix_.index_ = {1, 2, 0, 1, 2};
  // model.lp_.a_matrix_.value_ = {1.0, 3.0, 1.0, 2.0, 2.0};
/*
struct ll;

typedef struct {
    size_t size;
    char *val;
    ll *next;
} ll;
*/

/* 
typedef struct {
    int num_col, num_row, num_nz, sense;

    double col_cost[10], col_lower[10], col_upper[10], *row_lower, *row_upper, offset, a_value[10];

    int *a_start, *a_index;

    int nIndex[2];
    char *varNames[10];
} model_t;
*/

typedef struct {
        double *col_value, *col_dual, *row_value, *row_dual, objective_value;
        int *col_basis_status, *row_basis_status,
            run_status, model_status;
} sol;
sol* solveModel(model_t *m);
void printSol(sol *solution, model_t *mod);

#endif
