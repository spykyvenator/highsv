FLOAT [0-9]*[\.,]?[0-9]*

%{
	#include "interfaces/highs_c_api.h"

	extern int yylex(void);
	static void readVar(void *mod, char *text, const char state);
	static void setBounds(void *mod, const double val, char sign);

	int rowIndex[200], numNz = 0, numCol = 0, numRow = 0;
	double lastVal, sign = 1, rowVal[200];
	enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
	char state = COST;
	char lastVarName[10];
	void *model;
%}

%%

max|MAX { if (Highs_changeObjectiveSense(model, kHighsObjSenseMaximize)) puts("changing sense to max failed"); }
min|MIN { if (Highs_changeObjectiveSense(model, kHighsObjSenseMinimize)) puts("changing sense to min failed"); }
st	{ 
	if (state == COST) 
		state = AVAL; 
	else 
		puts("error: only one st allowed"); 
  numNz = 0;
	if (lastVal) {
		if (Highs_changeObjectiveOffset(model, lastVal)) puts("setting offset failed");
		lastVal = 0;
	    } 
	}

">"|">=" { state = BOUNDG; }// TODO: allow negative values
"<"|"<=" { state = BOUNDL; }
"="|"==" { state = BOUNDE; }

{FLOAT} { 
        if (state == BOUNDG) {
            setBounds(model, sign * atof(yytext), 1); 
            state = AVAL; 
            sign = 1;
        } else if (state == BOUNDL) {
            setBounds(model, sign * atof(yytext), -1);
            state = AVAL;
            sign = 1;
        } else if (state == BOUNDE) {
            setBounds(model, sign * atof(yytext), 0);
            state = AVAL;
            sign = 1;
        } else if (!lastVal) {
            lastVal = sign * atof(yytext); 
            sign = 1;
        } else {
            puts("don't put two floats behind each other"); 
        }
        }

\+ { sign = 1; }
\- { sign = -1; }

[a-zA-Z][0-9a-zA-Z]* { /*puts(yylex);*/ readVar(model, yytext, state); }

%%

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../print.h"
#include "../printOstream.h"

void
printModel(const void* mod)
{
  const HighsInt numCol = Highs_getPresolvedNumCol(mod), numRow = Highs_getPresolvedNumRow(mod);
  double offset;

  const double inf = Highs_getInfinity(mod);

  Highs_getObjectiveOffset(mod, &offset);
  printf("offset: %lf\n numCol: %d, numRow: %d\n", 
    offset,
    numCol, numRow
    );
}

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
setBounds (void *mod, const double val, char sign) {
	printf("setting bounds with numNz = %d\n", numNz);
  for (uint8_t i = 0; i < numNz; i++){
    printf("%o: %d, %lf \n", i, rowIndex[i], rowVal[i]);

  }
  puts("adding row\n");
  switch (sign) {
    case 1:// >=
      //Highs_changeRowBounds(mod, rowN, val, Highs_getInfinity(mod));
      Highs_addRow(mod, val, Highs_getInfinity(mod), numNz, rowIndex, rowVal);
      break;
    case 0:// =
      //Highs_changeRowBounds(mod, rowN, val, val);
      Highs_addRow(mod, val, val, numNz, rowIndex, rowVal);
      break;
    case -1:// <=
      //Highs_changeRowBounds(mod, rowN, -Highs_getInfinity(mod), val);
      Highs_addRow(mod, -Highs_getInfinity(mod), val, numNz, rowIndex, rowVal);
      break;
  }
	numNz = 0;
  numRow++;
}

/*
	add a new variable to the current row
*/
static void
readVar (void *mod, char *text, const char state) 
{
	puts("readvar:");
	puts(text);
	int index = findIndex(mod, text);
	if (lastVal == 0) // this should be ok
	    lastVal = sign;
	
	if (state == COST) {
		Highs_changeColCost(mod, index, lastVal);
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
  for (int i = 0; i < 200; i++){
    rowIndex[i] = 0;
    rowVal[i] = 0;
  }
}

static void
preModel (void *mod) 
{
	model = Highs_create();
}

int
parseString(const char *s, GOutputStream* ostream)
{
  puts(s);
  preModel(model);
  YY_BUFFER_STATE buffer = yy_scan_string(s);
  yylex();
  printModel(model);
  Highs_presolve(model);
  Highs_run(model);
  printSol(model);
  printSolToFile(model, ostream);
  yy_delete_buffer(buffer);
  cleanModel(model);
  return 0;
}
