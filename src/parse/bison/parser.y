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
	#include <string.h>
	#include "pt.c"

	void *model = NULL;
	int h_line = 0;
	int *rowIndex = NULL, numNz = 0;
	size_t rowLen = 2, numRow = 0, numCol = 0;
	double *rowVal = NULL;
	static void setCost(void *mod, const char *var, const double val);

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
%nterm <sm*> statement


%printer { fprintf (yyo, "%f", $$); } <double>
%left "+" "-"
%left "*" "/"
%left "^";

%%

%start input;

input: %empty
     | MAX cost eol ST constraints trailingEOL { highsv_setSenseMax(model); }
     | MIN cost eol ST constraints trailingEOL { highsv_setSenseMin(model); }
     ;

appcost: %empty
	| "+" cost
cost: %empty
   | expr VAR appcost { setCost(model, $2, $1); printf("%s: %f", $2, $1); }
   | VAR appcost { setCost(model, $1, 1.0); printf("%s: %f", $1, 1.0); }
   | expr { highsv_setObjectiveOffset(model, $1); }

constraints: %empty
	   | constraint EOL constraints

constraint: statement LESS statement {  mergeSm($1, $3); puts("less"); }
	   | statement MORE statement { mergeSm($1, $3); puts("more"); }
	   | statement EQUAL statement { mergeSm($1, $3); puts("equal"); }

eol: EOL { h_line++; printf("\nline: %d\n", h_line); }

statement: %empty { $$ = init_sm(); }
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

static void

static sm*
mergeSm(sm* a, sm* b)
{
}

static sm*
init_sm(void)
{
	sm* res = (sm*) h_malloc(sizeof(sm));
	res->offset = 0.0;
	res->rL = 2;
	res->vals = h_malloc(sizeof(double)*res->rL);
	res->indices = h_malloc(sizeof(size_t)*res->rL);
	return res;
}



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
    index = highsv_getNumCol(model);
    highsv_addVar(model);
    highsv_passColName(model, index, text);
  }
  return (size_t) index;
}

static void
setCost(void *mod, const char *var, const double val)
{
	const size_t index = findIndex(mod, var);
	highsv_changeColCost(model, index, val);
}

static void
setVal(void *mod, const char *var, const double val)
{
	const size_t index = findIndex(mod, var);
	if (index >= rowLen) {
		double *tmpVal = (double*) h_malloc(sizeof(double)*rowLen*2);
		int *tmpIndex = (int*) h_malloc(sizeof(int)*rowLen*2);
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
	rowVal[index] += val;
	rowIndex[numNz] = index;
	rowVal[numNz++] = val;
}

/*
static void
readVar(void *mod, char *text, double val)
{
  size_t index = findIndex(mod, text);

  if (index >= rowLen) {
    double *tmpVal = (double*) h_malloc(sizeof(double)*rowLen*2);
    int *tmpIndex = (int*) h_malloc(sizeof(int)*rowLen*2);
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
  rowVal[index] += lastVal;
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
*/

void
yyerror(const char *msg)
{
	die(msg);
}

int 
main(int argc, const char *argv[])
{
  yyparse();
  return 0;
}
