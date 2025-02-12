#include "interfaces/highs_c_api.h"
#include <stdarg.h>
#include <gtk/gtk.h>

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

static double
getRowConstraint(const void *mod, const HighsInt row)
{
  HighsInt nRow, nz, m_start, m_index[numCol];
  double lower, upper, m_value[numCol];
  Highs_getRowsByRange(mod, row, row, &nRow, &lower, &upper, &nz, &m_start, m_index, m_value);
  return ABS(lower) < ABS(upper) ? lower : upper;
}

static double*
getRowIntervals(const void *mod)
{
  double inf = Highs_getInfinity(mod);
  HighsInt c_index[numRow], nz;
  double c_vect[numRow][numCol], rc[numRow];
  for (size_t i = 0; i < numRow; i++){
    rc[i] = getRowConstraint(mod, (HighsInt) i);
    Highs_getBasisInverseRow(mod, i, c_vect[i], &nz, c_index);
  }

#ifdef DEBUG
  for (size_t i = 0; i < numRow; i++){// this c_vect is in fact correct
    for (size_t j = 0; j < numCol; j++)
      printf("%lf ", c_vect[i][j]);
    printf("%lf\n", rc[i]);
  }
#endif

  double *res = malloc(sizeof(double)*numRow*3);

  for (size_t i = 0; i < numRow; i++){// this is fine => There is probably a simpler way of doing this
    double div, lower, higher;
    double val = 0;
    res[i*3+1] = inf;
    res[i*3+2] = inf;
    for (size_t j = 0; j < numCol; j++){
      for (size_t k = 0; k < numRow; k++){
	if (i != k){
	  val += c_vect[j][k]*rc[k];
#ifdef DEBUG
	  printf("doing: %lf += %lf (%ld, %ld) * %lf\n", val, c_vect[j][k], j, k, rc[k]);
#endif //DEBUG
	} else{
	  div = c_vect[j][k];
#ifdef DEBUG
	  printf("setting div: %lf\n", div);
#endif //DEBUG
	}
      }
#ifdef DEBUG
      printf("%lf b%ld +%lf >=0\n", div, i, val);
#endif
      res[i*3] = rc[i];
      double result = ABS(ABS(val/div) - rc[i]);
      if (signbit(div) && result < res[i*3+1])// set decrease
	res[i*3+1] = result;
      else if (result < res[i*3+2])//set increase
	res[i*3+2] = result;
      val = 0;
    }
  }
  return res;
}

static void
pRange(void *mod, GOutputStream *ostr)
{
  char text[kHighsMaximumStringLength];
  double cost;
  HighsInt numResCol, numResNz, m_start, m_index[numCol];
  double resColLower[numCol], resColUpper[numCol], resColValue[numCol];
  double ccUpperVal[numCol], ccUpperObj[numCol], 
         ccLowerVal[numCol], ccLowerObj[numCol],
         cBndUpperVal[numCol], cBndUpperObj[numCol],
         cBndLowerVal[numCol], cBndLowerObj[numCol],
         rBndUpperVal[numRow], rBndUpperObj[numRow],
         rBndLowerVal[numRow], rBndLowerObj[numRow];
  double *rowInt = getRowIntervals(mod);
  HighsInt ccUpperInVar[numCol], ccUpperOutVar[numCol],
         ccLowerInVar[numCol], ccLowerOutVar[numCol],
         cBndUpInVar[numCol], cBndUpOutVar[numCol],
         cBndLowerInVar[numCol], cBndLowerOutVar[numCol],
         rBndUpperInVar[numRow], rBndUpperOutVar[numRow],
         rBndLowerInVar[numRow], rBndLowerOutVar[numRow];

  Highs_getRanging(mod, ccUpperVal, ccUpperObj, ccUpperInVar, ccUpperOutVar,// most of these prob can be NULL
      ccLowerVal, ccLowerObj, ccLowerInVar, ccLowerOutVar,
      cBndUpperVal, cBndUpperObj, cBndUpInVar, cBndUpOutVar,
      cBndLowerVal, cBndLowerObj, cBndLowerInVar, cBndLowerOutVar,
      rBndUpperVal, rBndUpperObj, rBndUpperInVar, rBndUpperOutVar,
      rBndLowerVal, rBndLowerObj, rBndLowerInVar, rBndLowerOutVar);

  pToF(ostr, "\n\t\tCurrent\t\tAllowable\t\tAllowable\nVariable\tCoefficient\tIncrease\t\tDecrease\n");
  for (size_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)
      break;
    Highs_getColsByRange(mod, (HighsInt) i, (HighsInt) i, &numResCol, &cost, resColLower, resColUpper, &numResNz, &m_start, m_index, resColValue);
    pToF(ostr, "%s\t\t%lf\t\t%lf\t\t%lf\n", text, cost, ABS(cost - ccUpperVal[i]), ABS(cost - ccLowerVal[i]));
  }
  pToF(ostr, "\n\t\tCurrent\t\tAllowable\t\tAllowable\nRow\t\tRHS\t\tIncrease\t\tDecrease\n");
  for (size_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)
      break;
    //double rC = getRowConstraint(mod, (HighsInt) i);
    pToF(ostr, "%s\t\t%lf\t\t%lf\t\t%lf\n", text, rowInt[i*3], rowInt[i*3+1], rowInt[i*3+2]);
  }
#ifdef DEBUG
for (size_t i = 0; i < numRow; i++) {
    pToF(ostr, "ccUpperVal: %lf\n, ccUpperVal%lf\n, ccUpperObj%lf\n, ccLowerVal%lf\n, ccLowerObj%lf\n, cBndUpperValue%lf\n, cBndUpperObj%lf\n, cBndLowerVal%lf\n, cBndLowerObj%lf\n, rBndUpperVal%lf\n, rBndUpperObj%lf\n, rBndLowerVal%lf\n, rBndLowerObj%lf", ccUpperVal[i], ccUpperObj[i], ccLowerVal[i], ccLowerObj[i], cBndUpperVal[i], cBndUpperObj[i], cBndLowerVal[i], cBndLowerObj[i], rBndUpperVal[i], rBndUpperObj[i], rBndLowerVal[i], rBndLowerObj[i]);
}
#endif //DEBUG
  free(rowInt);
}

static double
mkPos(double val){
    if (val == 0.0 && signbit(val))// remove negative 0's
        val += 0.0;
    return val;
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

    pToF(ostr, "%s\t\t%lf\t\t\t%lf\n", text, mkPos(col_value[i]), mkPos(col_dual[i]));
  }

  pToF(ostr, "\n\nRow\t\t Slack or Surplus\tDual Price\n%s\t\t%lf\t\t\t%lf\n", 
      "0", objectiveVal - offset, 0.0);

  /*
   * slack/surplus = abs(diff of row)
   */
  for (size_t i = 0; i < numRow; i++){
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
  HighsInt status = Highs_getModelStatus(mod);
  pToF(ostr, "Something went wrong: %d\n", status);
}

static void
pOpt(void *mod, GOutputStream *ostr){
  pToF(ostr, "Global optimal solution found:\n");
  pVal(mod, ostr);
  pRange(mod, ostr);
}

void
printSolToFile(void *mod, GOutputStream* ostr) {
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

