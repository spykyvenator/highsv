FLOAT [0-9]*[\.,]?[0-9]*

%{
typedef struct {
    int num_col, num_row, num_nz, sense = 0;

    double col_cost[10], col_lower[10], col_upper[10], row_lower[10], row_upper[10], offset, a_value[10];

    int *a_start, *a_index;

    int nIndex[2];
    char *varNames[10];
} model_t;
	extern int yylex(void);
	static void readVar(char *text, char cost);
	int i = 0;
	double lastVal;
	double values[32];
	double col_cost[32];
	double sign = 1;
	char cost = 1;
	model_t mod;
%}

%%

max|MAX	{ if (!mod.sense) mod.sense = -1; else puts("multiple min/max declarations"); }
min|MIN { if (!mod.sense) mod.sense = 1; else puts("multiple min/max declarations"); }
st { if (cost) cost = 0; else puts("error: only one st allowed"); i = 0; if (lastVal) {mod.offset = lastVal; lastVal = 0;} }

{FLOAT} { if (!lastVal) lastVal = sign * atof(yytext); else puts("don't put two floats behind each other"); }
[a-zA-Z][0-9a-zA-Z]* { readVar(yytext, cost); i++;}

\+ {sign = 1;}
\- {sign = -1;}

"> "{FLOAT}|">= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_lower[mod.num_row++] = atof(&(yytext[2])); }
"< "{FLOAT}|"<= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_upper[mod.num_row++] = atof(&(yytext[2])); }
"= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_upper[mod.num_row++] = atof(&(yytext[2])); mod.col_lower[mod.num_row] = mod.col_upper[mod.num_row]; }


%%

#include <stdlib.h>
#include <string.h>
#include "../model.h" 

#define MIN(a,b) (a < b ? a : b)

void
printModel(model_t *mod) 
{
	printf("offset == %lf\tnum_col == %d\tnum_row == %d\tsense == %d\n", mod->offset, mod->num_col, mod->num_row, mod->sense);

	for (int i = 0; i < mod->nIndex[0]; i++){
		printf("varNames[%d] == %s", i, mod->varNames[i]);
	}

	for (int i = 0; i < mod->num_col; i++) {
		printf("col_cost[%d] == %lf\t", i, mod->col_cost[i]);
		printf("col_lower[%d] == %lf\t", i, mod->col_lower[i]);
		printf("col_upper[%d] == %lf\t", i, mod->col_upper[i]);
		puts("\n");
	}
	for (int i = 0; i < mod->num_row; i++) {
		printf("row_lower[%d] == %lf\t", i, mod->row_lower[i]);
		printf("row_upper[%d] == %lf\t", i, mod->row_upper[i]);
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

	int i = 0;
	for (; i < mod.nIndex[0]; i++){
		size_t j = strlen(mod.varNames[i]);
		res = strncmp(text, mod.varNames[i], MIN(cmp, j));
		if (res == 0) {
			return i;
		}
	}
	return i;
}

static void
readVar(char *text, char cost) 
{
	int index = findIndex(text);
	if (cost) {
		mod.col_cost[index] = lastVal;
	} else {
		mod.a_value[index] = lastVal;
	}
	lastVal = 0;
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
	yylex();
	printModel(&mod);
}
