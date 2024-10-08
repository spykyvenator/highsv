/*
 * range printing with Highs_getRanging
 */
#ifndef PRINTO_H
#define PRINTO_H
#include <highs/interfaces/highs_c_api.h>
#include <stdarg.h>

extern size_t numRow, numCol;

static void
pToF(GOutputStream *ostr, const char *str, ...)
{
  gsize bw;
  GError *error = NULL;
  va_list ap;
  va_start(ap, str);
  if (!g_output_stream_vprintf(ostr, &bw, NULL, &error,
        str, ap)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
  va_end(ap);
}

static void
pEmpty(const void *mod, GOutputStream* ostr){
  pToF(ostr,"Your model appears empty\n");
}

static void
printInfeasible(const void *mod, GOutputStream* ostr)
{
  pToF(ostr, "Your model is infeasible\n");
}

static double
getSlack(const void *mod, const HighsInt row, const HighsInt num_nz, const double *r_value)
{
  int rowNumNz, matrix_index[num_nz];
  HighsInt nRow, start;
  double lower, upper, matrix_value[num_nz];
  Highs_getRowsByRange(mod, row, row, &nRow, &lower, &upper, 
        &rowNumNz, &start, 
        matrix_index, matrix_value);
  const double rowBound = MIN(ABS(lower), ABS(upper));
#ifdef DEBUG
  printf("row slack: %lf- %lf\n", r_value[row], rowBound);
#endif
  return ABS(r_value[row]-rowBound);

}

void
pRange(void *mod, GOutputStream *ostr)
{
  double ccUpperVal, ccUpperObj, 
         ccLowerVal, ccLowerObj,
         cBndUpperValue, cBndUpperObj,
         cBndLowerVal, cBndLowerObj,
         rBndUpperVal, rBndUpperObj,
         rBndLowerVal, rBndLowerObj;

  HighsInt ccUpperInVar, ccUpperOutVar,
         ccLowerInVar, ccLowerOutVar,
         cBndUpInVar, cBndUpOutVar,
         cBndLowerInVar, cBndLowerOutVar,
         rBndUpperInVar, rBndUpperOutVar,
         rBndLowerInVar, rBndLowerOutVar;

  Highs_getRanging(mod, &ccUpperVal, &ccUpperObj, &ccUpperInVar, &ccUpperOutVar,
      &ccLowerVal, &ccLowerObj, &ccLowerInVar, &ccLowerOutVar,
      &cBndUpperValue, &cBndUpperObj, &cBndUpInVar, &cBndUpOutVar,
      &cBndLowerVal, &cBndLowerObj, &cBndLowerInVar, &cBndLowerOutVar,
      &rBndUpperVal, &rBndUpperObj, &rBndUpperInVar, &rBndUpperOutVar,
      &rBndLowerVal, &rBndLowerObj, &rBndLowerInVar, &rBndLowerOutVar);
  pToF(ostr, "ccUpperVal: %lf\n, ccUpperVal%lf\n, ccUpperObj%lf\n, ccLowerVal%lf\n, ccLowerObj%lf\n, cBndUpperValue%lf\n, cBndUpperObj%lf\n, cBndLowerVal%lf\n, cBndLowerObj%lf\n, rBndUpperVal%lf\n, rBndUpperObj%lf\n, rBndLowerVal%lf\n, rBndLowerObj%lf", ccUpperVal, ccUpperObj, ccLowerVal, ccLowerObj, cBndUpperValue, cBndUpperObj, cBndLowerVal, cBndLowerObj, rBndUpperVal, rBndUpperObj, rBndLowerVal, rBndLowerObj);
}

/*
 * TODO: clean this function up and split it into multiple funcs
 */
static void
pVal(const void *mod, GOutputStream *ostr)
{
  gsize bw;
  GError *error = NULL;
  const double objectiveVal = Highs_getObjectiveValue(mod);
  char text[kHighsMaximumStringLength];
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow],
        col_reduced[numRow], reducedCost, offset;
  HighsInt col_index[numRow];
  const HighsInt num_nz = Highs_getNumNz(mod);

  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);
  Highs_getObjectiveOffset(mod, &offset);

  pToF(ostr, "Objective value:\t\t%lf\nTotal Variables:\t\t\t%d\nTotal Constraints:\t\t%d\nTotal nonzeros:\t\t\t%d\n\nVariable\tValue\t\tReduced Cost\n", 
       
      objectiveVal, numCol, numRow, num_nz);

  for (size_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;
    HighsInt reducedIndex;

#ifdef DEBUG
    printf("num_nz: %d col_dual %lf\n", num_nz, col_dual[i]);
#endif
    if (col_value[i] == 0.0 && signbit(col_value[i]))// remove negative 0's
        col_value[i] += 0.0;
    if (col_dual[i] == 0.0 && signbit(col_dual[i]))
        col_dual[i] += 0.0;

    pToF(ostr, "%s\t\t%lf\t\t\t%lf\n",text, col_value[i], col_dual[i]);
  }

  pToF(ostr, "\n\nRow\t\t Slack or Surplus\tDual Price\n%s\t\t%lf\t\t\t%lf\n", 
      "0", objectiveVal - offset, 0.0);

  /*
   * slack/surplus = abs(diff of row)
   */
  for (uint8_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;
#ifdef DEBUG
    printf("row_dual %lf,row_value %lf\n", row_dual[i], row_value[i]);
#endif
    if (row_value[i] == 0.0 && signbit(row_value[i]))
        row_value[i] += 0.0;
    if (row_dual[i] == 0.0 && signbit(row_dual[i]))
        row_dual[i] += 0.0;
    pToF(ostr, "%s\t\t%lf\t\t\t%lf\n", text,  getSlack(mod, i, num_nz, row_value), row_dual[i]);
  }
}

static void
pUnbound(const void *mod, GOutputStream *ostr){
  pToF(ostr, "Your model is unbounded\n");
  pVal(mod, ostr);
}

static void
pErr(const void *mod, GOutputStream *ostr){
  pToF(ostr, "Something went wrong\n");
}

static void
pOpt(const void *mod, GOutputStream *ostr){
  pToF(ostr, "Global optimal solution found:\n");
  pVal(mod, ostr);
}

static void
printSolToFile(const void *mod, GOutputStream* ostr) {
  HighsInt status = Highs_getModelStatus(mod);
  double time = Highs_getRunTime(mod);
  pToF(ostr, "HiGHS Version: %d.%d.%d\nTime: %lfs\n",
    Highs_versionMajor(), Highs_versionMinor(), Highs_versionPatch(), time);

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
    pErr(mod, ostr);
  else if (status == kHighsModelStatusModelEmpty)
    pEmpty(mod, ostr);
  else if (status == kHighsModelStatusOptimal)
    pOpt(mod, ostr);
  else if (status == kHighsModelStatusInfeasible 
      || status == kHighsModelStatusUnboundedOrInfeasible)
    printInfeasible(mod, ostr);
  else if (status == kHighsModelStatusUnbounded)
    pUnbound(mod, ostr);
}
#endif
