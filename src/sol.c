#include "interfaces/highs_c_api.h"
#include <gtk/gtk.h>
#include "./parse/parse.h"
#include "./pOstream.h"
#include "sol.h"

extern int *rowIndex, numNz;
extern size_t rowLen, numRow, numCol;
extern double lastVal, sign, *rowVal;
extern char state;
extern char lastVarName[10];
extern void *model;

void
cleanModel (void *mod)
{
  Highs_destroy(mod);
  numCol = 0;
  numRow = 0;
  state = COST;
  lastVal = 0;
  sign = 1,
  numNz = 0;
  if (rowVal){
    free(rowVal);
    rowVal = NULL;
  }
  if (rowIndex){
    free(rowIndex);
    rowIndex = NULL;
  }
  rowLen = 2;
}

void
preModel (void *mod) 
{
	model = Highs_create();
  rowVal = (double*) malloc(sizeof(double)*rowLen);
  rowIndex = (int*) malloc(sizeof(int)*rowLen);
  Highs_setBoolOptionValue(model, "log_to_console", 0);
  Highs_setBoolOptionValue(model, "output_flag", 0);
}

int
parseString(const char *s, GOutputStream* ostream)
{
  preModel(model);
  YY_BUFFER_STATE buffer = yy_scan_string(s);
  yylex();
  const double inf = Highs_getInfinity(model);
  const double z = 0;
  for (uint8_t i  = 0; i < numCol; i++)// make a switch for this
    Highs_changeColsBoundsByRange(model, i, i, &z, &inf);
#ifdef DEBUG
  printModel(model);
#endif
  Highs_presolve(model);
  Highs_run(model);
#ifdef DEBUG
  printSol(model);
#endif
  printSolToFile(model, ostream);
  yy_delete_buffer(buffer);
  cleanModel(model);
  return 0;
}
