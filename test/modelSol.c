#include "../src/model.h"

#include <stdlib.h>
#include <stdio.h>

int
main(void) 
{
    double c_cost[] = {1.0, 1.0};
    double c_lower[] = {0.0, 1.0};
    double c_upper[] = {4.0, 1.0e30};
    double r_lower[] = {-1.0e30, 5.0, 6.0};
    double r_upper[] = {7.0, 15.0, 1.0e30};
    int a_start[] = {0, 2};
    int a_index[] = {1, 2, 0, 1, 2};
    double a_val[] = {1.0, 3.0, 1.0, 2.0, 2.0};
    char *names[] = {"x1", "x2"};
    model_t in = {
        .num_col = 2,
        .num_row = 3,
        .num_nz = 5,

        .col_cost = c_cost,
        .col_lower = c_lower,
        .col_upper = c_upper,
        .row_lower = r_lower,
        .row_upper = r_upper,
        .a_start = a_start,
        .a_index = a_index,
        .a_value = a_val,

        .offset = 3,
        //.sense = kHighsObjSenseMinimize,
        .sense = 1, // minimize
        .varNames = names,
    };
    sol* out = solveModel(&in);
    printSol(out, &in);
    return 0;
}

