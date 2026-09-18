#ifdef HIGHS_BACKEND
#include "highs_interface.h"
#endif

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

#ifdef PREPROCESS
#include "../tp/vbpp/common.h"
#include "../tp/vbpp/yacc_stuff.h"
#include "../tp/vbpp/vpp_yacc.h"
#endif

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
  model = highsv_create();
  return 0;
}

int
quitModel ()
{
  return 0;
}

static void
preModel () 
{
  model = highsv_create();
#ifdef DEBUG
  printf("model: %p\n", model);
#endif
  highsv_setBoolOptionValue(model, "log_to_console", 0);
  highsv_setBoolOptionValue(model, "output_flag", 0);
}

int
parseString(const char *s, GOutputStream* ostream, errHandle *err)
{
    int res;
    yyscan_t scanner;

    cleanModel(model);
    preModel();
    HIGHSV_lex_init(&scanner);
    YY_BUFFER_STATE buffer = HIGHSV__scan_string(s, scanner);
#ifdef DEBUG
  yydebug=1;
#endif
    res = HIGHSV_parse(scanner, err);
#ifdef DEBUG
    printModel(model);
#endif
    highsv_presolve(model);
    clock_t before = clock();
    highsv_run(model);
    clock_t diff = clock() - before;
    printSolToStream(model, ostream, (double) diff/CLOCKS_PER_SEC);

    HIGHSV__delete_buffer(buffer, scanner);
    HIGHSV_lex_destroy(scanner);
    return res;
}

int
parseFile(FILE *fd, GOutputStream* ostream, errHandle *err)
{
    int res;
    yyscan_t scanner;

    cleanModel(model);
    preModel();
    HIGHSV_lex_init(&scanner);
    HIGHSV_set_in(fd, scanner);
#ifdef DEBUG
    yydebug=1;
#endif
    res = HIGHSV_parse(scanner, err);
#ifdef DEBUG
    printModel(model);
#endif
    highsv_presolve(model);
    clock_t before = clock();
    highsv_run(model);
    clock_t diff = clock() - before;
    printSolToStream(model, ostream, (double) diff/CLOCKS_PER_SEC);
    fclose(fd);

    HIGHSV_lex_destroy(scanner);
    return res;
}
