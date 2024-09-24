#ifndef PRINTO_H
#define PRINTO_H
#include <highs/interfaces/highs_c_api.h>
#include <stdarg.h>

extern int numRow, numCol;

//#define MIN(a,b) (a < b ? a : b)
//#define ABS(a) (a < 0 ? -a : a)

void
pToF(GOutputStream *ostream, const char *str, ...)
{
  gsize bw;
  GError *error = NULL;
  va_list ap;
  va_start(ap, str);
  if (!g_output_stream_vprintf(ostream, &bw, NULL, &error,
        str, ap)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
  va_end(ap);

}

void
printEmpty(const void *mod, GOutputStream* ostream){
  pToF(ostream,"Your model appears empty\n");
}

void
printInfeasible(const void *mod, GOutputStream* ostream){
  pToF(ostream, "Your model is infeasible\n");
}

/*
 * TODO: clean this function up and split it into multiple funcs
 */
void
printValues(const void *mod, GOutputStream *ostream)
{
  gsize bw;
  GError *error = NULL;
  const double objectiveVal = Highs_getObjectiveValue(mod);
  char text[kHighsMaximumStringLength];
  const char RowText[] = "\n\nRow\t\t Slack or Surplus\tDual Price\n";
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow],
        col_reduced[numRow], reducedCost, offset;
  HighsInt col_index[numRow];
  const HighsInt num_nz = Highs_getNumNz(mod);

  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);
  Highs_getObjectiveOffset(mod, &offset);

  pToF(ostream, "Objective value:\t\t%lf\nTotal Variables:\t\t\t%d\nTotal Constraints:\t\t%d\nTotal nonzeros:\t\t\t%d\n\nVariable\tValue\t\tReduced Cost\n", 
      objectiveVal, numCol, numRow, num_nz);

  for (uint8_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;
    HighsInt  reducedIndex;

    printf("num_nz: %d\n", num_nz);
    if (col_value[i] == 0.0 && signbit(col_value[i]))// remove negative 0's
        col_value[i] += 0.0;

    if (!col_value[i]){
      reducedCost = 0.0;
    } else {
      HighsInt colNz;
      if (Highs_getReducedColumn(mod, i, col_reduced, &colNz, col_index) == kHighsStatusError)
        break;
      reducedCost = col_reduced[0];
    }
    
    pToF(ostream, "%s\t\t%lf\t\t\t%lf\n",text, col_value[i], reducedCost);
  }

  pToF(ostream, RowText);
  pToF(ostream, "%s\t\t%lf\t\t\t%lf\n", "0", objectiveVal - offset, 0.0);

  /*
   * slack/surplus = abs(diff of row)
   */
  for (uint8_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;

    HighsInt numRow, start;
    int rowNumNz, matrix_index[num_nz];
    double lower, upper, matrix_value[num_nz];
    if (Highs_getRowsByRange(mod, i, i, &numRow, &lower, &upper, 
          &rowNumNz, &start, 
          matrix_index, matrix_value) == kHighsStatusError)
      break;
    const double rowBound = MIN(ABS(lower), ABS(upper));
    double rowVal = 0;
    for (int j = 0; j < rowNumNz; j++) {
      rowVal += matrix_value[j] * col_value[matrix_index[j]];
    }
    printf("row slack: %lf- %lf\n", rowVal, rowBound);


    pToF(ostream, "%s\t\t%lf\t\t\t%lf\n", text,  ABS(rowVal-rowBound), 0.0);
  }
}

void
printUnbounded(const void *mod, GOutputStream *ostream){
  pToF(ostream, "Your model is unbounded\n");
  printValues(mod, ostream);
}

void
printError(const void *mod, GOutputStream *ostream){
  pToF(ostream, "Something went wrong\n");
}

void
printOptimal(const void *mod, GOutputStream *ostream){
  pToF(ostream, "Global optimal solution found:\n");
  printValues(mod, ostream);
}

void
printSolToFile(const void *mod, GOutputStream* ostream) {
  HighsInt status = Highs_getModelStatus(mod);

  if (status == kHighsModelStatusNotset
    || status == kHighsModelStatusLoadError
    || status == kHighsModelStatusModelError
    || status == kHighsModelStatusPresolveError
    || status == kHighsModelStatusSolveError
    || status == kHighsModelStatusPostsolveError
    || status == kHighsModelStatusTimeLimit
    || status == kHighsModelStatusIterationLimit
    || status == kHighsModelStatusUnknown
    || status == kHighsModelStatusSolutionLimit
    || status == kHighsModelStatusInterrupt)
    printError(mod, ostream);
  else if (status == kHighsModelStatusModelEmpty)
    printEmpty(mod, ostream);
  else if (status == kHighsModelStatusOptimal)
    printOptimal(mod, ostream);
  else if (status == kHighsModelStatusInfeasible 
      || status == kHighsModelStatusUnboundedOrInfeasible)
    printInfeasible(mod, ostream);
  else if (status == kHighsModelStatusUnbounded)
    printUnbounded(mod, ostream);
}
#endif
