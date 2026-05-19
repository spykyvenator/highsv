%define parse.trace
%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}

%code requires {
	#include "pt.h"
	#include "../../sol.h"
	#include <math.h>
	//#include "../../print.h"
        #include "../../util.h"
    }

%code top {
	#include <stddef.h>

	void *model = NULL;
	int h_line = 0;
	int *rowIndex = NULL;
	size_t rowLen = 2, numRow = 0, numCol = 0;
	double *rowVal = NULL;
	extern char* yytext;

}
%code provides {	
#define YY_DECL                                 \
  yytoken_kind_t yylex(YYSTYPE* yylval)
  YY_DECL;
  void yyerror(const char *msg);
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
     | MAX costE st constraints trailingEOL { highsv_setSenseMax(model); }
     | MIN costE st constraints trailingEOL { highsv_setSenseMin(model); }
     ;

st: trailingEOLS ST trailingEOLS

costE: cost
     | cost expr { highsv_setObjectiveOffset(model, $2); }
cost: %empty
   | cost expr VAR { 
   	setCost(model, $3, $2); 
	//printf("setting cost: %s: %f", $2, $1); 
	char *name = $3; 
	free(name);// TODO: don't think this is necessary anymore in left recursive parsing
   }
   | cost VAR { 
   	setCost(model, $2, 1.0); 
	//printf(" setting cost: %s: %f", $1, 1.0); 
	char *name = $2; 
	free(name); 
   }
   ;

constraints: constraint
	   | constraints eol constraint { }// here we have to add the constraint to the model

constraint: statement LESS statement {  // <=
		  $$ = mergeSm($1, $3); 
		  destroy_sm($3); 
		  highsv_addRow(model, -highsv_getInfinity(model), $1->offset, $1->numNz, $1->indices, $1->vals);
		  char rowName[512];
		  snprintf(rowName, 512, "%ld", numRow + 1);
		  highsv_passRowName(model, numRow++, rowName);
		  #ifdef DEBUG
	  	  print_sm($1);
		  #endif
		  destroy_sm($1);
	   }
	   | statement MORE statement { // >=
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   highsv_addRow(model, $1->offset, highsv_getInfinity(model), $1->numNz, $1->indices, $1->vals);
		   char rowName[512];
		   snprintf(rowName, 512, "%ld", numRow + 1);
		   highsv_passRowName(model, numRow++, rowName);
		   #ifdef DEBUG
	  	   print_sm($1);
		   #endif
		   destroy_sm($1);
	   }
	   | statement EQUAL statement { // =
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   highsv_addRow(model, $1->offset, $1->offset, $1->numNz, $1->indices, $1->vals);
		  char rowName[512];
		  snprintf(rowName, 512, "%ld", numRow + 1);
		  highsv_passRowName(model, numRow++, rowName);
		   #ifdef DEBUG
		   print_sm($1);
		   #endif
		   destroy_sm($1);
	   }
	   ;


statement: %empty { 
	 #ifdef DEBUG
	 printf("init sm\n"); 
	 #endif
	 $$ = init_sm(); 
   }
   | expr VAR statement {
	   $$ = setVal(model, $3, $2, $1); 
	   char *name = $2;
	   free(name);
	   #ifdef DEBUG
	   printf("%s: %f\n", $2, $1); 
	   #endif
   }
   | VAR statement {
	   $$ = setVal(model, $2, $1, 1.0); 
	   char *name = $1;
	   free(name);
	   #ifdef DEBUG
	   printf("%s: %f\n", $1, 1.0); 
	   #endif
   }
   | expr {
	   #ifdef DEBUG
	   printf("init sm\n"); 
	   #endif
	   $$ = init_sm();
   	   $$->offset+=$1; 
	   #ifdef DEBUG
	   printf("%f\n", $1); 
	   #endif
   }
   ;

expr: NUM { $$ = $1; }
    | expr "+" expr { $$ = $1 + $3; }
    | expr "-" expr { $$ = $1 - $3; }
    | expr "*" expr { $$ = $1 * $3; }
    | expr "/" expr { $$ = $1 / $3; }
    | expr "^" expr { $$ = pow($1, $3); }
    | "(" expr ")" { $$ = $2; }
    ;

eol: EOL { 
   h_line++; 
   #ifdef DEBUG
   printf("\nline: %d\n", h_line); 
   #endif
   }
   ;
trailingEOLS: eol
	    | eol trailingEOLS ;// trailing start, require at least one eol
trailingEOL: %empty
	   | eol trailingEOL 
	   ;

%%

void
yyerror(const char *msg)
{
	die("parser stopped at line: %d because of %s at: %s", h_line, msg, yytext);
}

/*
int 
main(int argc, const char *argv[])
{
  model = highsv_create();
  initModel();
  yyparse();
  highsv_presolve(model);
  #ifdef DEBUG
  printModel(model);
  highsv_writeModel(model, "/tmp/model.lp");
  #endif
  return 0;
}
*/
