FLOAT [0-9]*[\.,]?[0-9]*

%{
	extern int yylex(void);
	static float readEntry(char *text);
	int i = 0;
	double values[32];
	double col_cost[32];
	double sign = 1;
	int sense = 0;
	int _lineNb = 0;
	struct model_t;
	char cost = 1;
	model_t mod;

%}

%%

max|MAX	{if (!sense) sense = -1; else puts("multiple min/max declarations");}
min|MIN {if (!sense) sense = 1; else puts("multiple min/max declarations");}
st {if(cost) cost = 0; else puts("error: only one st allowed");}

{FLOAT}\ *[a-zA-Z][0-9a-zA-Z]* {values[i++] = readEntry(yytext, cost);}

\+ {sign = 1;}
\- {sign = -1;}

"> "{FLOAT}|">= "{FLOAT} {lineNb++; if (i > nbcols) nbcols = i; i = 0;}
"< "{FLOAT}|"<= "{FLOAT} {lineNb++; if (i > nbcols) nbcols = i; i = 0;}
"= "{FLOAT} {lineNb++; if (i > nbcols) nbcols = i; i = 0;}


%%

#include <stdlib.h>
#include "../model.h" 

static float
readEntry(char *text, char cost;) 
{
	char *endptr;
	float value = strtof(text, &endptr);
	if (*text != '0' && value == 0)
		value = 1;
	value *= sign;
	while (*endptr == ' ') endptr++;
	printf("value: %f: from %s", value, endptr);
	return value;
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
