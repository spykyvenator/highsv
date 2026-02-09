%define parse.trace
%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}

%code top {
	#include "../../highs_interface.h"
	#include <stddef.h>
	#include <math.h>
	void *model;
	int h_line = 0;
}
%code provides {
#define YY_DECL                                 \
  yytoken_kind_t yylex(YYSTYPE* yylval)
  YY_DECL;
  void yyerror(const char *msg);
  static size_t findIndex(void *mod, const char *text);
}

%token 
	MAX
	MIN
	ST
	EOL
	MORE
	LESS
	EQUAL
;

%token <double> NUM "number"
%token <char *> VAR "var"
%nterm <double> expr


%printer { fprintf (yyo, "%f", $$); } <double>
%left "+" "-"
%left "*" "/"
%left "^";

%%

%start input;

input: %empty
     | MAX optfunc eol ST eol constraints trailingEOL { highsv_setSenseMax(model); }
     | MIN optfunc eol ST eol constraints trailingEOL { highsv_setSenseMin(model); }
     ;

optfunc: %empty
   | expr VAR optfunc { printf("%s: %f", $2, $1); }
   | VAR optfunc { printf("%s: %f", $1, 1.0); }
   | expr { printf("%f", $1); }

constraints: %empty
	   | constraint EOL constraints

constraint: statement LESS statement { puts("less"); }
	   | statement MORE statement { puts("more"); }
	   | statement EQUAL statement { puts("more"); }

eol: EOL { h_line++; printf("\nline: %d\n", h_line); }

statement: %empty
   | expr VAR statement { printf("%s: %f", $2, $1); }
   | VAR statement { printf("%s: %f", $1, 1.0); }
   | expr { printf("%f", $1); }

expr: NUM { $$ = $1; }
    | expr "+" expr { $$ = $1 + $3; }
    | expr "-" expr { $$ = $1 - $3; }
    | expr "*" expr { $$ = $1 + $3; }
    | expr "/" expr { $$ = $1 - $3; }
    | expr "^" expr { $$ = pow($1, $3); }

trailingEOL: %empty
	   | EOL trailingEOL
	   ;

%%

/*
  return the column index of a variable name.
  add it if it is not present
*/
static size_t
findIndex(void *mod, const char *text)
{
  int64_t index;
  if (highsv_getColByName(mod, text, &index) == HIGHSV_STATUS_ERROR) {// returns Error when col does not exist
    #ifdef DEBUG
    printf("adding %s to index\n", text);
    #endif
    index = highsv_getNumCol(mod);
    highsv_addVar(mod, -Highs_getInfinity(mod), Highs_getInfinity(mod));
    highsv_passColName(model, index, text);
  }
  return (size_t) index;
}

static void
readVar(void *mod, char *text, const char state, double val)
{
  size_t index = findIndex(mod, text);

  if (index >= rowLen) {
    double *tmpVal = (double*) malloc(sizeof(double)*rowLen*2);
    int *tmpIndex = (int*) malloc(sizeof(int)*rowLen*2);
    memcpy(tmpVal, rowVal, sizeof(double)*rowLen);
    memcpy(tmpIndex, rowIndex, sizeof(int)*rowLen);
    free(rowVal);
    free(rowIndex);
    rowVal = tmpVal;
    rowIndex = tmpIndex;
    for (size_t i = rowLen; i < rowLen*2; i++) {// allocate to zero
      rowVal[i] = 0;
      rowIndex[i] = 0;
    }
    rowLen*=2;
  }
	if (state == COST) {
#ifdef DEBUG
		printf("readvar: %s, %f\n", text, lastVal);
#endif
		rowVal[index] += lastVal;
		Highs_changeColCost(mod, index, rowVal[index]);
	} else if (state == AVAL) {
		rowIndex[numNz] = index;
		rowVal[numNz++] = lastVal;
	}
	lastVal = 0;
	strcpy(lastVarName, text);
}

void
yyerror(const char *msg)
{
	puts(msg);
}

int 
main(int argc, const char *argv[])
{
  yyparse();
  return 0;
}
