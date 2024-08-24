FLOAT [0-9]*[\.,]?[0-9]*

%{

	#include "../model.h" 
	extern int yylex(void);
	static void readVar(char *text, const char state);
	static void setBounds(double val, char sign);

	int i = 0, rowIndex[200];
	double lastVal, sign = 1, rowVal[200];
	enum { COST, AVAL, BOUNDG, BOUNDL, BOUNDE};
	char state = COST;
	char lastVarName[10];
	void *model;
%}

%%

max|MAX	{ mod.num_col = i; printf("set modnumCol %d, %d\n", i, mod.num_col); i = 0; if (!mod.sense) mod.sense = -1; else puts("multiple min/max declarations"); }
min|MIN	{ mod.num_col = i; printf("set modnumCol %d, %d\n", i, mod.num_col); i = 0; if (!mod.sense) mod.sense = 1; else puts("multiple min/max declarations"); }
st	{ 
	if (state == COST) 
		state = AVAL; 
	else 
		puts("error: only one st allowed"); 
	i = 0; 
	if (lastVal) {
		mod.offset = lastVal; 
		lastVal = 0;
	    } 
	}

">"|">=" { state = BOUNDG; }// TODO: allow negative values
"<"|"<=" { state = BOUNDL; }
"="|"==" { state = BOUNDE; }

{FLOAT} { 
        if (state == BOUNDG) {
            setBounds(sign * atof(yytext), 1); 
            state = AVAL; 
            sign = 1;
        } else if (state == BOUNDL) {
            setBounds(sign * atof(yytext), -1);
            state = AVAL;
            sign = 1;
        } else if (state == BOUNDE) {
            setBounds(sign * atof(yytext), 0);
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

[a-zA-Z][0-9a-zA-Z]* { i++; readVar(yytext, state); }


%%

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

//#define MIN(a,b) (a < b ? a : b)

void
printModel(model_t *mod) 
{
	printf("offset == %lf\tnum_col == %d\tnum_row == %d\tnum_nz == %d\tsense == %d\n", mod->offset, mod->num_col, mod->num_row, mod->num_nz, mod->sense);

	for (int i = 0; i < mod->nIndex[0]; i++){
		printf("varNames[%d] == %s\n", i, mod->varNames[i]);
	}

	for (int i = 0; i < mod->num_col; i++) {
		printf("col_cost[%d] == %lf\t", i, mod->col_cost[i]);
		printf("col_lower[%d] == %lf\t", i, mod->col_lower[i]);
		printf("col_upper[%d] == %lf\t", i, mod->col_upper[i]);
		printf("a_start[%d] == %d", i, mod->a_start[i]);
		puts("\n");
	}
	for (int i = 0; i < mod->num_row; i++) {
		printf("row_lower[%d] == %lf\t", i, mod->row_lower[i]);
		printf("row_upper[%d] == %lf\t", i, mod->row_upper[i]);
		puts("\n");
	}
	for (int i = 0; i < mod->num_nz; i++) {
		printf("a_value[%d] == %lf\t", i, mod->a_value[i]);
		printf("a_index[%d] == %d\t", i, mod->a_index[i]);
		puts("\n");
	}
	
}

/*
 This should be ok
*/
int
findIndex(const char *text)
{
	HighsInt index;
	if (getColByName(model, text, &index) == KHighsStatusError) {// returns Error when col does not exist
		index = Highs_getNumCol(model);
		Highs_passColName(model, ++index, text);
	}
	return index;
}

static void
setBounds(double val, char sign) {
	printf("setting bounds with i = %d ", i);
	if (i == 1) {// set col_upper/lower instead of the row_lower/upper
		int index = findIndex(lastVarName);
		printf("setting columns to %lf index: %d name: %s\n", val, index, lastVarName);
   //TODO: check if new col limit is bigger/smaller than current before setting
		switch (sign) {// this works correctly
			case 1:// >=
				mod.col_lower[index] = val/a_value_cache[index][--a_value_index[index]];
				break;
			case 0:// =
				mod.col_upper[index] = val/a_value_cache[index][--a_value_index[index]];
				mod.col_lower[index] = val/a_value_cache[index][--a_value_index[index]];
				break;
			case -1:// <= 
				mod.col_upper[index] = val/a_value_cache[index][--a_value_index[index]];
				break;
		}

		i = 0;
		if (mod.num_nz > 0){
		    mod.num_nz--;
		}
		return;
	}
	if (i > mod.num_col) 
		mod.num_col = i; 
	i = 0; 
	switch (sign) {// its here that our negatives fail 
		case 1:
			mod.row_lower[mod.num_row] = val; 
			mod.row_upper[mod.num_row++] = 1.0e30;// Highs_getInfinity();
			break;
		case 0:
			mod.row_lower[mod.num_row] = val; 
			mod.row_upper[mod.num_row++] = val;
			break;
		case -1:
			mod.row_lower[mod.num_row] = -1.0e30; 
			mod.row_upper[mod.num_row++] = val;
			break;
	}
	int index = findIndex(lastVarName);
	mod.a_start[index]++;
}


static void
readVar(char *text, const char state) 
{
	puts("readvar:");
	puts(text);
	int index = findIndex(text);
	if (lastVal == 0) {// this should be ok
	    lastVal = sign;
	}
	if (state == COST) {
		mod.col_cost[index] = lastVal;
	} else if (state == AVAL) {
		a_value_cache[index][a_value_index[index]++] = lastVal;
		mod.num_nz++;
	}
	lastVal = 0;
	strcpy(lastVarName, text);
}

static void
completeModel (model_t *mod) 
{
	mod->a_start[0] = 0;
	for (int i = 1; i < mod->num_col; i++) {
		mod->a_start[i]+= mod->a_start[i-1];
	}
	int n = 0;
	for (int i = 0; i < mod->num_col; i++) {
		for (int j = 0; j < a_value_index[i]; j++) {
			mod->a_index[n] = j;
			mod->a_value[n++] = a_value_cache[i][j];
		}
	}
}

static void
cleanModel (model_t *mod)
{

        for (int i = 0; i < mod->nIndex[0]; i++) {
                free(mod->varNames[i]);
                mod->varNames[i] = NULL;
        }

	for (int i = 0; i < 200; i++) {
                mod->col_cost[i] = 0;
                mod->row_lower[i] = 0;
                mod->row_upper[i] = 0;
                mod->a_value[i] = 0;
                mod->a_start[i] = 0;
                mod->a_index[i] = 0;
	}

        mod->nIndex[0] = 0;
        mod->num_col = 0;
        mod->num_row = 0;
        mod->num_nz = 0;
        mod->sense = 0;
        mod->offset = 0;
        state = COST;
        lastVal = 0;
        i = 0;
        sign = 1;
        for (int i = 0; i < 10; i++) {
            lastVarName[i] = '\0';
        }

        for (int i = 0; i < 50; i++) {
            for (int j = 0; j < 100; j++) {
                a_value_cache[i][j] = 0;
            }
        }
        for (int i = 0; i < 100; i++) {
            a_value_index[i] = 0;
        }
	Highsv_destroy(model);
}

static void
preModel (model_t *mod) 
{
	model = Highs_create();

	for (int i = 0; i < 200; i++) {
		mod->col_upper[i] = 1.0e30;
		mod->col_lower[i] = -1.0e30;

                mod->row_lower[i] = 0;
                mod->row_upper[i] = 0;
                mod->a_value[i] = 0;
                mod->a_start[i] = 0;
                mod->a_index[i] = 0;
	}
}

int
parseStdin() {
	mod.a_format = 2;
	preModel(&mod);
        cleanModel(&mod);
	yylex();
	completeModel(&mod);
	printModel(&mod);
	sol* res = solveModel(&mod);
	//printSol(res, &mod);
}

int
parseString(const char *s, GOutputStream* ostream)
{
        puts(s);
	mod.a_format = 2;
	preModel(&mod);
	YY_BUFFER_STATE buffer = yy_scan_string(s);
	yylex();
	completeModel(&mod);
	printModel(&mod);
	sol* res = solveModel(&mod);
	printSolToFile(res, &mod, ostream);
	yy_delete_buffer(buffer);
        cleanModel(&mod);
	return 0;
}
