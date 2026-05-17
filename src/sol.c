#include "highs_interface.h"

#ifdef CLI
#include <stdio.h>
#include "./cli/cli.h"
#include <glib.h>
#else
#include <gtk/gtk.h>
#endif
#include <time.h>

#include "./parse/bison/parser.h"
#include "./parse/bison/scanner.h"
#include "./pOstream.h"
#include "sol.h"
#ifdef DEBUG
#include "print.h"
#endif

extern int *rowIndex;
extern size_t rowLen, numRow, numCol;
//extern double lastVal, sign, *rowVal;
extern char state;
//extern char lastVarName[255];
extern void *model;

static void
cleanModel (void *model)
{
  highsv_destroy(model);
  numCol = 0;
  numRow = 0;
}

int
initModel ()
{
  //rowIndex = (int*) malloc(sizeof(int)*rowLen);
  model = highsv_create();
  return 0;
}

int
quitModel ()
{
  //if (rowVal) {
   // free(rowVal);
    //rowVal = NULL;
  //}
  if (rowIndex) {
    free(rowIndex);
    rowIndex = NULL;
  }
  return 0;
}

static void
preModel () 
{
  model = highsv_create();
  //for (size_t i = 0; i < rowLen; i++) {// init to zero
      //rowVal[i] = 0;
      //rowIndex[i] = 0;
  //}
  printf("model: %p\n", model);
  highsv_setBoolOptionValue(model, "log_to_console", 0);
  highsv_setBoolOptionValue(model, "output_flag", 0);
}

static void
setPositive(char pos, void *model)
{
  if (pos) {
    const double inf = highsv_getInfinity(model);
    double infinity[numCol];
    double zero[numCol];
    for (size_t i = 0; i < numCol; i++) {
      infinity[i] = inf;
      zero[i] = 0;
    }
    highsv_changeColsBoundsByRange(model, 0, numCol-1, zero, infinity);
  }
}

static void
setMip(char mip, void *model)
{
  if (mip) {
    int64_t integrality[numCol];
    for (size_t i = 0; i < numCol; i++) integrality[i] = HIGHSV_T_INT;
    highsv_changeColsIntegralityByRange(model, 0, numCol-1, integrality);
  }
}

int
parseString(const char *s, GOutputStream* ostream, gboolean mip, gboolean pos)
{
  cleanModel(model);
  preModel();
  YY_BUFFER_STATE buffer = yy_scan_string(s);
  yyparse();
#ifdef DEBUG
  printModel(model);
#endif
  setPositive((char) pos, model);
  setMip((char) mip, model);
  highsv_presolve(model);
  clock_t before = clock();
  highsv_run(model);
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
  yyset_in(fd);// add yyscanner here for reentrant
  yyparse();
#ifdef DEBUG
  printModel(model);
#endif
  setPositive((char) pos, model);
  setMip(mip, model);
  highsv_presolve(model);
  clock_t before = clock();
  highsv_run(model);
  clock_t diff = clock() - before;
  printSolToFile(model, ostream, (double) diff/CLOCKS_PER_SEC);
  fclose(fd);
  highsv_writeModel(model, "/tmp/model.lp");
  return 0;
}
