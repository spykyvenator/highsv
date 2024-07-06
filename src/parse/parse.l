FLOAT [0-9]*[\.,]?[0-9]*

%{

	#include "../model.h" 
	extern int yylex(void);
	static void readVar(char *text, char cost);
	static void setBounds(double val, char sign);

	int i = 0, a_value_index[10];
	double lastVal, sign = 1, a_value_cache[10][10];
	char cost = 1;
	model_t mod;
	char lastVarName[10];
%}

%%

max|MAX	{ mod.num_col = i; printf("set modnumCol %d, %d\n", i, mod.num_col); i = 0; if (!mod.sense) mod.sense = -1; else puts("multiple min/max declarations"); }
min|MIN	{ mod.num_col = i; printf("set modnumCol %d, %d\n", i, mod.num_col); i = 0; if (!mod.sense) mod.sense = 1; else puts("multiple min/max declarations"); }
st	{ 
	if (cost) 
		cost = 0; 
	else 
		puts("error: only one st allowed"); 
	i = 0; 
	if (lastVal) {
		mod.offset = lastVal; 
		lastVal = 0;
	    } 
	}

">"\ *{FLOAT}|">= "{FLOAT} { setBounds( atof(&(yytext[2])), 1); }
"<"\ *{FLOAT}|"<= "{FLOAT} { setBounds( atof(&(yytext[2])), -1); }
"="\ *{FLOAT} { setBounds( atof(&(yytext[2])), 0); }

{FLOAT} { if (!lastVal) lastVal = sign * atof(yytext); else puts("don't put two floats behind each other"); }

\+ {sign = 1;}
\- {sign = -1;}

[a-zA-Z][0-9a-zA-Z]* { i++; readVar(yytext, cost); }


%%

#include <stdlib.h>
#include <string.h>

#define MIN(a,b) (a < b ? a : b)

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

size_t
strlen(const char *s) {
	size_t i = 0;
	while (s[i++] != '\0');
	return i;
}

int
findIndex(char *text)
{
	size_t cmp = strlen(text);
	int res;

	for (int i = 0; i < mod.nIndex[0]; i++){
		size_t j = strlen(mod.varNames[i]);
		res = strncmp(text, mod.varNames[i], MIN(cmp, j));
		if (res == 0) {
			return i;
		}
	}
	mod.varNames[mod.nIndex[0]] = malloc(cmp);
	strcpy(mod.varNames[mod.nIndex[0]], text);
	return mod.nIndex[0]++;
}


/*
 TODO: check if new col limit is bigger/smaller than current before setting
*/
static void
setBounds(double val, char sign) {
	printf("setting bounds with i = %d ", i);
	if (i == 1) {// set col_upper/lower instead of the row_lower/upper
		int index = findIndex(lastVarName);
		printf("settinc columns to %lf index: %d name: %s\n", val, index, lastVarName);
		switch (sign) {
			case 1:// >=
				//mod.col_lower[index] = val/mod.a_value[mod.num_nz-1]; 
				//mod.col_upper[index] = 1.0e30;
				mod.col_lower[index] = val/a_value_cache[index][--a_value_index[index]];
				break;
			case 0:// =
				//mod.col_lower[index] = val/mod.a_value[mod.num_nz-1]; 
				//mod.col_upper[index] = val/mod.a_value[mod.num_nz-1];
				mod.col_upper[index] = val/a_value_cache[index][--a_value_index[index]];
				mod.col_lower[index] = val/a_value_cache[index][--a_value_index[index]];
				break;
			case -1:// <= 
				//mod.col_lower[index] = -1.0e30; 
				//mod.col_upper[index] = val/mod.a_value[mod.num_nz-1];
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
	switch (sign) {
		case 1:
			mod.row_lower[mod.num_row] = val; 
			mod.row_upper[mod.num_row++] = 1.0e30;
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
readVar(char *text, char cost) 
{
	puts("readvar:");
	puts(text);
	int index = findIndex(text);
	if (lastVal == 0) {// this should be ok
	    lastVal = 1;
	}
	if (cost) {
		mod.col_cost[index] = lastVal;
	} else  {
		//mod.a_index[mod.num_nz] = mod.num_row;
		//mod.a_index[mod.num_nz] = mod.num_row;
		//mod.a_value[mod.num_nz++] = lastVal;
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
preModel (model_t *mod) 
{
	for (int i = 0; i < 10; i++) {
		mod->col_upper[i] = 1.0e30;
		mod->col_lower[i] = -1.0e30;
	}
}

/*
static int
yylex(int *lineNb, char **vars) {
	int res = yylex();
	*lineNb = _lineNb;
	return yylex();
}
*/

int
main () {
	mod.a_format = 2;
	preModel(&mod);
	yylex();
	completeModel(&mod);
	printModel(&mod);
	sol* res = solveModel(&mod);
	printSol(res, &mod);
}
