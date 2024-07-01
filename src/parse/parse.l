FLOAT [0-9]*[\.,]?[0-9]*

%{
typedef struct {
    int num_col, num_row, num_nz, sense;

    double col_cost[10], col_lower[10], col_upper[10], *row_lower, *row_upper, offset, a_value[10];

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
	int sense = 0;
	int _lineNb = 0;
	char cost = 1;
	model_t mod;
%}

%%

max|MAX	{ if (!sense) sense = -1; else puts("multiple min/max declarations"); }
min|MIN { if (!sense) sense = 1; else puts("multiple min/max declarations"); }
st { if (cost) cost = 0; else puts("error: only one st allowed"); i = 0; if (lastVal) {mod.offset = lastVal; lastVal = 0;} }

{FLOAT} { if (!lastVal) lastVal = sign * atof(yytext); else puts("don't put two floats behind each other"); }
[a-zA-Z][0-9a-zA-Z]* { readVar(yytext, cost); }

\+ {sign = 1;}
\- {sign = -1;}

"> "{FLOAT}|">= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_lower[_lineNb++] = atof(&(yytext[2])); }
"< "{FLOAT}|"<= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_upper[_lineNb++] = atof(&(yytext[2])); }
"= "{FLOAT} { if (i > mod.num_col) mod.num_col = i; i = 0; mod.col_upper[_lineNb++] = atof(&(yytext[2])); mod.col_lower[_lineNb] = mod.col_upper[_lineNb]; }


%%

#include <stdlib.h>
#include <string.h>
#include "../model.h" 

#define MIN(a,b) (a < b ? a : b)

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
struct model_t*
readFile ()
{
	yyin == fopen(name, "r");
	yylex();
}
*/

struct model_t*
readStdIn ()
{

}

/*
static int
yylex(int *lineNb, char **vars) {
	int res = yylex();
	*lineNb = _lineNb;
	return yylex();
}
*/

/*
int
main () {
        int i = 3;
        char * var = "x1";
	yylex();
	printf("index: %d\n", i);
	for (int j = 0; j < i; j++)
		printf("value: %f\n", values[j]);
}
*/
