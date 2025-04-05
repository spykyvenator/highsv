#include "interfaces/highs_c_api.h"
#include <stdarg.h>

#ifdef CLI
#include "./cli/cli.h"
#include <gio/gio.h>
#include <glib.h>
#include <glib/gprintf.h>
#else
#include <gtk/gtk.h>
#endif

#include "./tprint/tprint.h"
#include <glib.h>
#include <stdio.h>


extern size_t numRow, numCol;

static void
pToF(GOutputStream *ostr, const char *str, ...)
{
  va_list ap;
  va_start(ap, str);
  gsize bw;
  GError *error = NULL;
  if (!g_output_stream_vprintf(ostr, &bw, NULL, &error,
        str, ap)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
  va_end(ap);
}

static double
mkPos(double val){
    if (val == 0.0 && signbit(val))// remove negative 0's
        val += 0.0;
    return val;
}


static void
pEmpty(GOutputStream* ostr){
  pToF(ostr,"Your model appears empty\n");
}

static void
printInfeasible(GOutputStream* ostr)
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
  const double rowBound = ABS(lower) < ABS(upper) ? lower : upper;
#ifdef DEBUG
  printf("row slack: %lf- %lf\n", r_value[row], rowBound);
#endif
  return ABS(r_value[row]-rowBound);
}

/*
 * Get the constraint that is limiting the row
 * Since one will always be infinite and the other not this is ok
 */
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
  for (size_t i = 0; i < numRow; i++) {
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
    double div, val = 0;
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

/*
 * TODO: split this function up in multiple funcs
 */
static void
pRange(void *mod, GOutputStream *ostr)
{
  const HighsInt num_nz = Highs_getNumNz(mod);
  char text[kHighsMaximumStringLength];
  HighsInt numResCol, numResNz, m_start[numCol], m_index[num_nz];
  double resColLower[numCol], resColUpper[numCol], resColValue[num_nz], cost[numCol];
  double ccUpperVal[numCol], ccUpperObj[numCol], 
         ccLowerVal[numCol], ccLowerObj[numCol],
         cBndUpperVal[numCol], cBndUpperObj[numCol],
         cBndLowerVal[numCol], cBndLowerObj[numCol],
         rBndUpperVal[numRow], rBndUpperObj[numRow],
         rBndLowerVal[numRow], rBndLowerObj[numRow];
  double *rowInt = getRowIntervals(mod);// in fact this should be calculated by taking the right hand side of the expression and caculating the distance to the current lefthand side
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

  TPrint *tp;
  tp = tprint_create (ostr, 0, 1, 0, 5);
  tprint_column_add (tp, "", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Current", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "Variable");
  tprint_data_add_str(tp, 1, "Coefficient");
  tprint_data_add_str(tp, 2, "Increase");
  tprint_data_add_str(tp, 3, "Decrease");

  Highs_getColsByRange(mod, 0, numCol - 1, &numResCol, cost, resColLower, resColUpper, &numResNz, m_start, m_index, resColValue);
  if ((size_t) numResCol != numCol) return;
  for (size_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError) return;
    tprint_data_add_str(tp, 0, text);
    tprint_data_add_double(tp, 1, cost[i]);
    tprint_data_add_double(tp, 2, ABS(cost[i] - ccUpperVal[i]));
    tprint_data_add_double(tp, 3, ABS(cost[i] - ccLowerVal[i]));
  }
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");

  tp = tprint_create (ostr, 0, 1, 0, 5);
  tprint_column_add (tp, "", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Current", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "Row");
  tprint_data_add_str(tp, 1, "RHS");
  tprint_data_add_str(tp, 2, "Increase");
  tprint_data_add_str(tp, 3, "Decrease");
  for (size_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)
      break;
    //double rC = getRowConstraint(mod, (HighsInt) i);
    tprint_data_add_str(tp, 0, text);
    tprint_data_add_double(tp, 1, mkPos(rowInt[i*3]));
    tprint_data_add_double(tp, 2, mkPos(rowInt[i*3+1]));
    tprint_data_add_double(tp, 3, mkPos(rowInt[i*3+2]));
  }
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");

#ifdef DEBUG
for (size_t i = 0; i < numRow; i++) {
    pToF(ostr, "ccUpperVal: %9.9lf\n, ccUpperVal%9.9lf\n, ccUpperObj%9.9lf\n, ccLowerVal%9.9lf\n, ccLowerObj%9.9lf\n, cBndUpperValue%9.9lf\n, cBndUpperObj%9.9lf\n, cBndLowerVal%9.9lf\n, cBndLowerObj%9.9lf\n, rBndUpperVal%9.9lf\n, rBndUpperObj%9.9lf\n, rBndLowerVal%9.9lf\n, rBndLowerObj%9.9lf", ccUpperVal[i], ccUpperObj[i], ccLowerVal[i], ccLowerObj[i], cBndUpperVal[i], cBndUpperObj[i], cBndLowerVal[i], cBndLowerObj[i], rBndUpperVal[i], rBndUpperObj[i], rBndLowerVal[i], rBndLowerObj[i]);
}
#endif //DEBUG
  free(rowInt);
}

/*
 * TODO: clean this function up and split it into multiple funcs
 */
static void
pVal(const void *mod, GOutputStream *ostr)
{
  const double objectiveVal = Highs_getObjectiveValue(mod);
  char text[kHighsMaximumStringLength];
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow],
        offset;
  const HighsInt num_nz = Highs_getNumNz(mod);

  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);
  Highs_getObjectiveOffset(mod, &offset);

  TPrint *tp;
  tp = tprint_create (ostr, 0, 0, 0, 5);
  tprint_column_add (tp, "", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "Objective Value:");
  tprint_data_add_double(tp, 1, objectiveVal);
  tprint_data_add_str(tp, 0, "Total Variables:");
  tprint_data_add_double(tp, 1, numCol);
  tprint_data_add_str(tp, 0, "Total Constraints:");
  tprint_data_add_double(tp, 1, numRow);
  tprint_data_add_str(tp, 0, "Total nonzeros:");
  tprint_data_add_double(tp, 1, num_nz);
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");

  tp = tprint_create (ostr, 0, 1, 0, 5);
  tprint_column_add (tp, "Variable", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Value", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Reduced Cost", TPAlign_left, TPAlign_left);

  for (size_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;

#ifdef DEBUG
    printf("num_nz: %d col_dual %9.9lf\n", num_nz, col_dual[i]);
#endif

    tprint_data_add_str(tp, 0, text);
    tprint_data_add_double(tp, 1, mkPos(col_value[i]));
    tprint_data_add_double(tp, 2, -col_dual[i]);
  }
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");

  /*
   * slack/surplus = abs(diff of row)
   */
  tp = tprint_create (ostr, 0, 1, 0, 5);
  tprint_column_add (tp, "Row", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Slack or Surplus", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Dual Price", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "0");
  tprint_data_add_double(tp, 1, mkPos(objectiveVal - offset));
  tprint_data_add_double(tp, 2, 1.0);

  for (size_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;
#ifdef DEBUG
    printf("row_dual %lf,row_value %lf\n", row_dual[i], row_value[i]);
#endif
    tprint_data_add_str(tp, 0, text);
    tprint_data_add_double(tp, 1, mkPos(getSlack(mod, i, num_nz, row_value)));
    tprint_data_add_double(tp, 2, row_dual[i]);
  }
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");
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
  TPrint *tp;
  tp = tprint_create (ostr, 0, 0, 0, 5);
  tprint_column_add(tp, "", TPAlign_left, TPAlign_left);
  tprint_column_add(tp, "", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "HiGHS Version:");
  char bfr[7];
  snprintf(bfr, 6, "%d.%d.%d", Highs_versionMajor(), Highs_versionMinor(), Highs_versionPatch());
  bfr[6] = '\0';
  tprint_data_add_str(tp, 1, bfr);
  tprint_data_add_str(tp, 0, "Time:");
  tprint_data_add_double(tp, 1, Highs_getRunTime(mod));
  tprint_print(tp);
  tprint_free(tp);

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
     pErr(mod, ostr);
   else if (status == kHighsModelStatusModelEmpty)
     pEmpty(ostr);
   else if (status == kHighsModelStatusOptimal)
     pOpt(mod, ostr);
   else if (status == kHighsModelStatusInfeasible
       || status == kHighsModelStatusUnboundedOrInfeasible)
     printInfeasible(ostr);
   else if (status == kHighsModelStatusUnbounded)
     pUnbound(mod, ostr);
}
