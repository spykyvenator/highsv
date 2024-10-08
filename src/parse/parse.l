FLOAT [0-9]*[\.,]?[0-9]*

%{
	#include "interfaces/highs_c_api.h"

	extern int yylex(void);
	static void readVar(void *mod, char *text, const char state);
	static void setBounds(void *mod, const double val, char sign);

	int *rowIndex = NULL, numNz = 0;
  size_t rowLen = 2, numRow = 0, numCol = 0;
	double lastVal, sign = 1, *rowVal = NULL;
	enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
	char state = COST;
	char lastVarName[10];
	void *model;
%}

%%

!.*\n { 
	#ifdef DEBUG
	printf("skipping comment: %s", yytext); 
	#endif
      }
max|MAX { if (Highs_changeObjectiveSense(model, kHighsObjSenseMaximize)) 
            puts("changing sense to max failed"); 
            }
min|MIN { if (Highs_changeObjectiveSense(model, kHighsObjSenseMinimize)) 
            puts("changing sense to min failed"); 
            }
st	{ 
	if (state == COST) 
		state = AVAL; 
#ifdef DEBUG
	else 
		puts("error: only one st allowed"); 
#endif
  numNz = 0;
	if (lastVal) {
		if (Highs_changeObjectiveOffset(model, lastVal)) puts("setting offset failed");
		lastVal = 0;
	    } 
	}

">"|">=" { state = BOUNDG; /* sign = 1; */ }// TODO: allow negative values
"<"|"<=" { state = BOUNDL; /* sign = 1; */ }
"="|"==" { state = BOUNDE; /* sign = 1; */ }

{FLOAT} { 
        if (state == BOUNDG) {
            setBounds(model, sign * atof(yytext), 1); 
            state = AVAL; 
        } else if (state == BOUNDL) {
            setBounds(model, sign * atof(yytext), -1);
            state = AVAL;
        } else if (state == BOUNDE) {
#ifdef DEBUG
	    printf("found double: %s to %lf, sign %lf\n", yytext, atof(yytext), sign);
#endif
            setBounds(model, sign * atof(yytext), 0);
            state = AVAL;
        } else if (!lastVal) {
            lastVal = sign * atof(yytext); 
        } 
#ifdef DEBUG
        else {
            puts("don't put two floats behind each other"); 
        }
#endif
	sign = 1;
        }

\+ { sign = 1; }
\- { sign = -1; }

[a-zA-Z][0-9a-zA-Z]* { readVar(model, yytext, state); }

%%

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
//#include "../print.h"
#include "../printOstream.h"

/*
  return the column index of a variable name.
  add it if it is not present
*/
int
findIndex(void *mod, const char *text)
{
  HighsInt index;
  if (Highs_getColByName(mod, text, &index) == kHighsStatusError) {// returns Error when col does not exist
    index = Highs_getNumCol(mod);
    Highs_addVar(mod, -Highs_getInfinity(mod), Highs_getInfinity(mod));
    Highs_passColName(mod, index, text);
    numCol++;
  }
  return index;
}

/*
	set the bounds for a column/row 
	depending on the amount of variables in the current text row
*/
static void
setBounds (void *mod, const double val, const char sign) 
{
  char rowName[kHighsMaximumStringLength];
#ifdef DEBUG
  printf("setting bounds with numNz = %d to %lf, sign %hhd\n", numNz, val, sign);
  for (uint8_t i = 0; i < numNz; i++){
    printf("setting bounds value: %o: index: %d, val: %lf \n", i, rowIndex[i], rowVal[i]);
  }
  puts("adding row\n");
#endif
  switch (sign) {
    case 1:// >=
      Highs_addRow(mod, val, Highs_getInfinity(mod), numNz, rowIndex, rowVal);
      break;
    case 0:// =
      Highs_addRow(mod, val, val, numNz, rowIndex, rowVal);
      break;
    case -1:// <=
      Highs_addRow(mod, -Highs_getInfinity(mod), val, numNz, rowIndex, rowVal);
      break;
  }
  snprintf(rowName, 512, "%ld", numRow + 1);
  Highs_passRowName(mod, numRow, rowName);
  numNz = 0;
  numRow++;
}

/*
	add a new variable to the current row
*/
static void
readVar (void *mod, char *text, const char state) 
{
#ifdef DEBUG
	puts("readvar:");
	puts(text);
#endif
	int index = findIndex(mod, text);
	if (lastVal == 0){
	    lastVal = sign;
	    sign = 1;
	}

  if (index >= rowLen){
    rowLen*=2;
    double *tmpVal = (double*) malloc(sizeof(double)*rowLen);
    int *tmpIndex = (int*) malloc(sizeof(int)*rowLen);
    memcpy(tmpVal, rowVal, sizeof(double)*numCol);
    memcpy(tmpIndex, rowIndex, sizeof(int)*numCol);
    free(rowVal);
    free(rowIndex);
    rowVal = tmpVal;
    rowIndex = tmpIndex;
  }
	if (state == COST) {
		rowVal[index] += lastVal;
		Highs_changeColCost(mod, index, rowVal[index]);
	} else if (state == AVAL) {
		rowIndex[numNz] = index;
		rowVal[numNz++] = lastVal;
	}
	lastVal = 0;
	strcpy(lastVarName, text);
}

static void
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

static void
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
