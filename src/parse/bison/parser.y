%define parse.trace
%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}

%code requires {
	#include "pt.h"
}

%code top {
	#include <stddef.h>
	#include <math.h>
	#include <string.h>

	void *model = NULL;
	int h_line = 0;
	int *rowIndex = NULL, numNz = 0;
	size_t rowLen = 2, numRow = 0, numCol = 0;
	double *rowVal = NULL;

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
%nterm <sm*> statement constraint


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

//cost: statement
cost: %empty
   | expr VAR cost { 
   	setCost(model, $2, $1); 
	printf("setting cost: %s: %f", $2, $1); 
	char *name = $2; 
	free(name);
   }
   | VAR cost { 
   	setCost(model, $1, 1.0); 
	printf(" setting cost: %s: %f", $1, 1.0); 
	char *name = $1; 
	free(name); 
   }
   | expr { 
   	highsv_setObjectiveOffset(model, $1); 
	printf("setting offset %f", $1); 
   }

constraints: %empty
	   | constraint eol constraints { 
		apply_sm($1, model);
		destroy_sm($1);
	   }// here we have to add the constraint to the model

constraint: statement LESS statement {  
		  $$ = mergeSm($1, $3); 
		  destroy_sm($3); 
		  puts("less"); 
	  }
	   | statement MORE statement { 
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   puts("more"); 
	   }
	   | statement EQUAL statement { 
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   puts("equal"); 
	   }

eol: EOL { h_line++; printf("\nline: %d\n", h_line); }

statement: %empty { $$ = init_sm(); }
   | expr VAR statement { 
	   $$ = setVal(model, $3, $2, $1); 
	   printf("%s: %f", $2, $1); 
   }
   | VAR statement { 
	   $$ = setVal(model, $2, $1, 1.0); 
	   printf("%s: %f", $1, 1.0); 
   }
   | expr { 
	   $$ = init_sm();
   	   $$->offset+=$1; 
	   printf("%f", $1); 
   }

expr: NUM { $$ = $1; }
    | expr "+" expr { $$ = $1 + $3; }
    | expr "-" expr { $$ = $1 - $3; }
    | expr "*" expr { $$ = $1 + $3; }
    | expr "/" expr { $$ = $1 - $3; }
    | expr "^" expr { $$ = pow($1, $3); }
    | "(" expr ")" { $$ = $2; }

trailingEOL: %empty
	   | EOL trailingEOL
	   ;

%%

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
  model = highsv_create();
  yyparse();
  return 0;
}
