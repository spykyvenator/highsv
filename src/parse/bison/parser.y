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
	int *rowIndex = NULL, numNz = 0;
	size_t rowLen = 2, numRow = 0, numCol = 0;
	double *rowVal = NULL;

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
     | MAX cost eol ST eol constraints trailingEOL { highsv_setSenseMax(model); }
     | MIN cost eol ST eol constraints trailingEOL { highsv_setSenseMin(model); }
     ;

//cost: statement
cost: %empty
   | expr VAR cost { 
   	setCost(model, $2, $1); 
	//printf("setting cost: %s: %f", $2, $1); 
	char *name = $2; 
	free(name);
   }
   | VAR cost { 
   	setCost(model, $1, 1.0); 
	//printf(" setting cost: %s: %f", $1, 1.0); 
	char *name = $1; 
	free(name); 
   }
   | expr { 
   	highsv_setObjectiveOffset(model, $1); 
	//printf("setting offset %f", $1); 
   }
   ;

constraints: %empty
	   | constraint eol constraints { }// here we have to add the constraint to the model

constraint: statement LESS statement {  // <=
		  $$ = mergeSm($1, $3); 
		  destroy_sm($3); 
		  puts("less"); 
		  highsv_addRow(model, -highsv_getInfinity(model), $1->offset, $1->numNz, $1->indices, $1->vals);
	  	  print_sm($1);
		  destroy_sm($1);
	   }
	   | statement MORE statement { // >=
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   puts("more"); 
		   highsv_addRow(model, $1->offset, highsv_getInfinity(model), $1->numNz, $1->indices, $1->vals);
	  	   print_sm($1);
		   destroy_sm($1);
	   }
	   | statement EQUAL statement { // =
		   $$ = mergeSm($1, $3); 
		   destroy_sm($3); 
		   puts("equal"); 
		   highsv_addRow(model, $1->offset, $1->offset, $1->numNz, $1->indices, $1->vals);
		   print_sm($1);
		   destroy_sm($1);
	   }
	   ;

eol: EOL { 
   h_line++; 
   #ifdef DEBUG
   printf("\nline: %d\n", h_line); 
   #endif
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
	   #ifdef DEBUG
	   printf("%s: %f\n", $2, $1); 
	   #endif
   }
   | VAR statement {
	   $$ = setVal(model, $2, $1, 1.0); 
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

trailingEOL: %empty
	   | EOL trailingEOL
	   ;

%%

void
yyerror(const char *msg)
{
	die(msg);
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
