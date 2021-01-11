FLOAT [0-9]*[\.,]?[0-9]*

%{
	#include "interfaces/highs_c_api.h"

	extern int yylex(void);
	static void readVar(void *mod, char *text, const char state);
	static void setBounds(void *mod, const double val, char sign);

	int i = 0, rowIndex[200], rowN = 0, colN = 0, numNz = 0;
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

[a-zA-Z][0-9a-zA-Z]* { i++; readVar(model, yytext, state); }


%%

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

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
		Highs_addCol(mod, 0, -Highs_getInfinity(mod), Highs_getInfinity(mod), 0, &index, NULL);
		Highs_passColName(mod, index, text);

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
	/*
	if (i == 1) {// set col_upper/lower instead of the row_lower/upper
		const int index = findIndex(mod, lastVarName);
		printf("setting columns to %lf index: %d name: %s\n", val, index, lastVarName);
   //TODO: check if new col limit is bigger/smaller than current before setting
		switch (sign) {// this works correctly
			case 1:// >=
				 Highs_changeColBounds(mod, index, val, Highs_getInfinity(mod));
				break;
			case 0:// =
				 Highs_changeColBounds(mod, index, val, val);
				break;
			case -1:// <= 
				 Highs_changeColBounds(mod, index, -Highs_getInfinity(mod), val);
				break;
		}

		i = 0;
		if (numNz > 0){
		    numNz--;
		}
		return;
	} else {
	*/
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
	//}
	numNz = 0;
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
}

static void
preModel (void *mod) 
{
	model = Highs_create();
	for (int i = 0; i < 200; i++){
		rowIndex[i] = 0;
		rowVal[i] = 0;
	}
}

int
parseString(const char *s, GOutputStream* ostream)
{
	preModel(model);
	YY_BUFFER_STATE buffer = yy_scan_string(s);
	yylex();
  printModel(model);
	Highs_presolve(model);
	Highs_run(model);
	//printSolToFile(res, &mod, ostream);
	yy_delete_buffer(buffer);
  cleanModel(model);
	return 0;
}
