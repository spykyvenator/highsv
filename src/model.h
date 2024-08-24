#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>
#include <gtk/gtk.h>


  // a_start_ has num_col_1 entries, and the last entry is the number
  // of nonzeros in A, allowing the number of nonzeros in the last
  // column to be defined
  // model.lp_.a_matrix_.start_ = {0, 2, 5};
  // model.lp_.a_matrix_.index_ = {1, 2, 0, 1, 2};
  // model.lp_.a_matrix_.value_ = {1.0, 3.0, 1.0, 2.0, 2.0};

    // a_start_ has num_col_1 entries, and the last entry is the number
    // of nonzeros in A, allowing the number of nonzeros in the last
    // column to be defined
    // it is thus the index of each column start in A

    // a_index are the indices in our rows of values in a_value
    // The position in a_index/a_value of the index/value of the first nonzero in each vector is stored in a_start
    // a_start[0] = 0

sol* solveModel(model_t *m);
void printSol(sol *solution, model_t *mod);
void printSolToFile(sol *solution, model_t *mod, GOutputStream* ostream);

#endif
