#include "interfaces/highs_c_api.h"
#include <gtk/gtk.h>
#include "./parse/parse.h"
#include "./pOstream.h"
#include "sol.h"
#ifdef DEBUG
#include "print.h"
#endif

extern int *rowIndex, numNz;
extern size_t rowLen, numRow, numCol;
extern double lastVal, sign, *rowVal;
extern char state;
extern char lastVarName[255];
extern void *model;

static void
cleanModel ()
{
  Highs_destroy(model);
  numCol = 0;
  numRow = 0;
  state = COST;
  lastVal = 0;
  sign = 1,
  numNz = 0;
  //rowLen = 2;
}

int
initModel ()
{
  rowVal = (double*) malloc(sizeof(double)*rowLen);
  rowIndex = (int*) malloc(sizeof(int)*rowLen);
  return 0;
}

int
quitModel ()
{
  //Highs_destroy(model);
  if (rowVal){
    free(rowVal);
    rowVal = NULL;
  }
  if (rowIndex){
    free(rowIndex);
    rowIndex = NULL;
  }
  return 0;
}

static void
preModel () 
{
  model = Highs_create();
  for (size_t i = 0; i < rowLen; i++) {// init to zero
      rowVal[i] = 0;
      rowIndex[i] = 0;
  }
  Highs_setBoolOptionValue(model, "log_to_console", 0);
  Highs_setBoolOptionValue(model, "output_flag", 0);
}

int
parseString(const char *s, GOutputStream* ostream, gboolean mip, gboolean pos)
{
  preModel();
  YY_BUFFER_STATE buffer = yy_scan_string(s);
  yylex();
#ifdef DEBUG
  printModel(model);
#endif
  const double inf = Highs_getInfinity(model);
  if (pos) {
    double infinity[numCol];
    double zero[numCol];
    for (size_t i = 0; i < numCol; i++) {
      infinity[i] = inf;
      zero[i] = 0;
    }
    Highs_changeColsBoundsByRange(model, 0, numCol-1, zero, infinity);
  }
  if (mip) {
    HighsInt integrality[numCol];
    for (size_t i = 0; i < numCol; i++) integrality[i] = kHighsVarTypeInteger;
    HighsInt res = Highs_changeColsIntegralityByRange(model, 0, numCol-1, integrality);
    if (res)
      return 1;
  }
  Highs_presolve(model);
  Highs_run(model);
  printSolToFile(model, ostream);
  yy_delete_buffer(buffer);
  cleanModel(model);
  return 0;
}
