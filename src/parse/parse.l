FLOAT [0-9]*[\.,]?[0-9]*

%{
typedef struct {
    int num_col, num_row, num_nz, sense;// num_nz is the number of nonzeros in a_value ( its lenght )

    double col_cost[10], col_lower[10], col_upper[10], row_lower[10], row_upper[10], a_value[10], offset;

    // a_start_ has num_col_1 entries, and the last entry is the number
    // of nonzeros in A, allowing the number of nonzeros in the last
    // column to be defined

    // a_index are the indices in our rows of values in a_value
    // The position in a_index/a_value of the index/value of the first nonzero in each vector is stored in a_start
    // a_start[0] = 0


    int a_start[10], a_index[10];

    int nIndex[2];
    char *varNames[10];
} model_t;

	extern int yylex(void);
	static void readVar(char *text, char cost);
	static void setBounds(double val, char sign);

	int i = 0;
	double lastVal, sign = 1;
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
#include "../model.h" 

#define MIN(a,b) (a < b ? a : b)

void
printModel(model_t *mod) 
{
	printf("offset == %lf\tnum_col == %d\tnum_row == %d\tsense == %d\n", mod->offset, mod->num_col, mod->num_row, mod->sense);

	for (int i = 0; i < mod->nIndex[0]; i++){
		printf("varNames[%d] == %s\n", i, mod->varNames[i]);
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
			case 1:
				mod.col_lower[index] = val; 
				mod.col_upper[index] = 1.0e30;
				break;
			case 0:
				mod.col_lower[index] = val; 
				mod.col_upper[index] = val;
				break;
			case -1:
				mod.col_lower[index] = -1.0e30; 
				mod.col_upper[index] = val;
				break;
		}

		i = 0;
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
}


static void
readVar(char *text, char cost) 
{
	puts("readvar:");
	puts(text);
	int index = findIndex(text);
	if (cost) {
		mod.col_cost[index] = lastVal;
	} else {
		mod.a_value[index] = lastVal;
	}
	lastVal = 0;
	strcpy(lastVarName, text);
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
