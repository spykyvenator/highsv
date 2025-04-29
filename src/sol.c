#include "interfaces/highs_c_api.h"

#ifdef CLI
#include <stdio.h>
#include "./cli/cli.h"
#include <glib.h>
#else
#include <gtk/gtk.h>
#endif
#include <time.h>

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
cleanModel (void *model)
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
  //Highs_setStringOptionValue(model, "presolve", "off");
}

int
setPositive(char pos, void *model)
{
  if (pos) {
    const double inf = Highs_getInfinity(model);
    double infinity[numCol];
    double zero[numCol];
    for (size_t i = 0; i < numCol; i++) {
      infinity[i] = inf;
      zero[i] = 0;
    }
    HighsInt res = Highs_changeColsBoundsByRange(model, 0, numCol-1, zero, infinity);
    return res;
  }
  return 0;
}

int
setMip(char mip, void *model)
{
  if (mip) {
    HighsInt integrality[numCol];
    for (size_t i = 0; i < numCol; i++) integrality[i] = kHighsVarTypeInteger;
    HighsInt res = Highs_changeColsIntegralityByRange(model, 0, numCol-1, integrality);
    return res;
  }
  return 0;
}

int
parseString(const char *s, GOutputStream* ostream, gboolean mip, gboolean pos)
{
  cleanModel(model);
  preModel();
  YY_BUFFER_STATE buffer = yy_scan_string(s);
  yylex();
#ifdef DEBUG
  printModel(model);
#endif
  setPositive((char) pos, model);
  setMip((char) mip, model);
  Highs_presolve(model);
  clock_t before = clock();
  Highs_run(model);
  clock_t diff = clock() - before;
  printSolToFile(model, ostream, (double) diff/CLOCKS_PER_SEC);
  yy_delete_buffer(buffer);
  return 0;
}

int
parseFile(FILE *fd, GOutputStream* ostream, char mip, char pos)
{
  cleanModel(model);
  preModel();
  yyset_in(fd);
  yylex();
#ifdef DEBUG
  printModel(model);
#endif
  setPositive(pos, model);
  setMip(mip, model);
  Highs_presolve(model);
  clock_t before = clock();
  Highs_run(model);
  clock_t diff = clock() - before;
  printSolToFile(model, ostream, (double) diff/CLOCKS_PER_SEC);
  fclose(fd);
  return 0;
}
