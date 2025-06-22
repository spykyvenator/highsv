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
mkPos(double val)
{
    if (val == 0.0 && signbit(val))// remove negative 0's
        val += 0.0;
    return val;
}


static void
pEmpty(GOutputStream* ostr)
{
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
getRowConstraint(const void *mod, const HighsInt row, char *type)
{
  HighsInt nRow, nz, m_start, m_index[numCol];
  double lower, upper, m_value[numCol];
  Highs_getRowsByRange(mod, row, row, &nRow, &lower, &upper, &nz, &m_start, m_index, m_value);
  if (type) 
      *type = ABS(lower) == ABS(upper) ? '=' : (ABS(lower) < ABS(upper) ? '<' : '>');
  return ABS(lower) < ABS(upper) ? lower : upper;
}

static double*
getLHS(const void *mod, HighsInt *resRows)
{
    const HighsInt num_nz = Highs_getNumNz(mod);
    HighsInt res_nz, m_start[numRow], m_index[num_nz];
    double lower[numRow], upper[numRow], m_value[num_nz], col_val[numCol], col_dual[numCol], row_val[numRow], row_dual[numRow];

    Highs_getRowsByRange(mod, 0, numRow - 1, resRows, 
            lower, upper, &res_nz, m_start, m_index, m_value);
    Highs_getSolution(mod, col_val, col_dual, row_val, row_dual);
    double *res = malloc(sizeof(double)* (*resRows));
    for (HighsInt i = 0; i < *resRows-1; i++) {
        res[i] = 0;
        for (HighsInt j = m_start[i]; j < m_start[i+1]; j++) {
            res[i] += col_val[m_index[j]]*m_value[j];
        }
    }
    for (HighsInt j = m_start[*resRows-1]; j < res_nz; j++) {
        res[*resRows-1] += col_val[m_index[j]]*m_value[j];
    }
    return res;
}

static double*
getDualPriceRanges(void *mod)
{
    HighsInt nbRows;
    double* lhs = getLHS(mod, &nbRows);
    double rc;
    double rowUBnd[numRow], rowLBnd[numRow];
    double *res = malloc(sizeof(double)*numRow*2);

    Highs_getRanging(mod, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      rowUBnd, NULL, NULL, NULL, rowLBnd, NULL, NULL, NULL);
    for (HighsInt i = 0; i < nbRows; i++) {
        //res[i*2] = 0;// 0 init
        //res[i*2+1] = 0;
        char type;
        rc = getRowConstraint(mod, (HighsInt) i, &type);
        if (rc == lhs[i]) {// we are a constraining row
            res[i*2] = ABS(rc - rowUBnd[i]);
            res[i*2+1] = ABS(rc - rowLBnd[i]);
        } else {
            switch (type) {
                case '=':
                    if (rc > lhs[i]){
                        res[i*2] = Highs_getInfinity(mod);// increase
                        res[i*2+1] = ABS(rc - lhs[i]);// decrease
                    } else{
                        res[i*2] = ABS(rc - lhs[i]);// increase
                        res[i*2+1] = Highs_getInfinity(mod);// decrease
                    } break;
                case '>':
                    res[i*2] = Highs_getInfinity(mod);
                    res[i*2+1] = ABS(rc - lhs[i]);
                    break;
                case '<':
                    res[i*2] = ABS(rc - lhs[i]);
                    res[i*2+1] = Highs_getInfinity(mod);
                    break;
                default:
                    res[i*2] = NAN;
                    res[i*2+1] = NAN;
                    break;
            }
        }
    }
    free(lhs);
    return res;
}

static double*
getRowIntervals(const void *mod)
{
  double inf = Highs_getInfinity(mod);
  //double (*c_vect) = malloc(sizeof(double)*numCol*numRow);// TODO: remove this malloc and replace with in place calls to Highs
  //if (c_vect == NULL) {
      //puts("ERROR: no more memory");
  //}
  double c_vect[numRow];
  double rc[numRow];
  for (size_t i = 0; i < numRow; i++) {
    rc[i] = getRowConstraint(mod, (HighsInt) i, NULL);
  }

  double *res = malloc(sizeof(double)*numRow*3);

  for (size_t i = 0; i < numRow; i++){// this is fine => There is probably a simpler way of doing this
    double div, val = 0;
    res[i*3+1] = inf;
    res[i*3+2] = inf;
    for (size_t j = 0; j < numCol; j++){
      for (size_t k = 0; k < numRow; k++){
        Highs_getBasisInverseRow(mod, j, c_vect, NULL, NULL);
	if (i != k){
	  val += c_vect[k]*rc[k];
#ifdef DEBUG_PRINTER
	  printf("doing: %lf += %lf (%ld, %ld) * %lf\n", val, c_vect[k], j, k, rc[k]);
#endif //DEBUG_RINTER
	} else{
	  div = c_vect[k];
#ifdef DEBUG_PRINTER
	  printf("setting div: %lf\n", div);
#endif //DEBUG_INTER
	}
      }
#ifdef DEBUG_PRINTER
      printf("%lf b%ld +%lf >=0\n", div, i, val);
#endif
      res[i*3] = rc[i];
      double result = ABS(ABS(val/div) - rc[i]);
      if (signbit(div) && result < res[i*3+1])// set decrease
	res[i*3+1] = mkPos(result);
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
  double *dualRng = getDualPriceRanges(mod);
  const HighsInt num_nz = Highs_getNumNz(mod);
  char text[kHighsMaximumStringLength];
  HighsInt numResCol, numResNz, m_start[numCol], m_index[num_nz];
  double resColLower[numCol], resColUpper[numCol], resColValue[num_nz], cost[numCol];
  double rowUp[numRow], rowDn[numRow], ccUpperVal[numCol], 
         ccLowerVal[numCol];
  double *rowInt = getRowIntervals(mod);

  Highs_getRanging(mod, ccUpperVal, NULL, NULL, NULL,
      ccLowerVal, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      rowUp, NULL, NULL, NULL, rowDn, NULL, NULL, NULL);

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
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_column_add (tp, "Allowable", TPAlign_left, TPAlign_left);
  tprint_data_add_str(tp, 0, "Row");
  tprint_data_add_str(tp, 1, "RHS");
  tprint_data_add_str(tp, 2, "Increase");
  tprint_data_add_str(tp, 3, "Decrease");
  tprint_data_add_str(tp, 4, "Increase");
  tprint_data_add_str(tp, 5, "Decrease");
  for (size_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)
      break;
    //double rC = getRowConstraint(mod, (HighsInt) i);
    tprint_data_add_str(tp, 0, text);
    tprint_data_add_double(tp, 1, mkPos(rowInt[i*3]));
    tprint_data_add_double(tp, 2, mkPos(rowInt[i*3+1]));
    tprint_data_add_double(tp, 3, mkPos(rowInt[i*3+2]));
    tprint_data_add_double(tp, 4, dualRng[i*2]);
    tprint_data_add_double(tp, 5, dualRng[i*2+1]);
  }
  tprint_print(tp);
  tprint_free(tp);
  pToF(ostr, "\n");

  g_free(dualRng);
  g_free(rowInt);
}

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
printSolToFile(void *mod, GOutputStream* ostr, double time) {
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
  tprint_data_add_double(tp, 1, time);
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
