%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}
%define locations

%code requires {
	#include "pt.h"
	#include "../../sol.h"
	#include <math.h>
        #include "../../util.h"
	typedef void * yyscan_t;
    }

%code top {
	#include <stddef.h>

	void *model = NULL;
	int h_line = 0;
	int *rowIndex = NULL;
	size_t rowLen = 2, numRow = 0, numCol = 0;
	double *rowVal = NULL;
}

%param {yyscan_t scanner}

%code provides {	
	#define YY_DECL \
	int yylex(YYSTYPE* yylval_param, YYLTYPE *yylloc, yyscan_t yyscanner)
	YY_DECL;
	void yyerror(YYLTYPE *yylloc, yyscan_t scanner, const char *msg);
}

%token 
	MAX
	MIN
	ST
	EOL
	MORE
	LESS
	EQUAL
	PLUS
        SUB
        MULT
        DIV
        POW
	BO
	BC
;

%token <double> NUM "number"
%token <char *> VAR "var"
%nterm <double> expr
%nterm <sm*> statement constraint


%printer { fprintf (yyo, "%f", $$); } <double>
%left PLUS SUB
%left MULT DIV
%left POW;

%%

%start input;

input: %empty
     | MAX statement st constraints trailingEOL { highsv_setSenseMax(model); setObjective(model, $2); }
     | MIN statement st constraints trailingEOL { highsv_setSenseMin(model); setObjective(model, $2); }
     ;

st: trailingEOLS ST trailingEOLS

constraints: constraint
	   | constraints EOL constraint { }// here we have to add the constraint to the model

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
    | expr PLUS expr { $$ = $1 + $3; }
    | expr SUB expr { $$ = $1 - $3; }
    | expr MULT expr { $$ = $1 * $3; }
    | expr DIV expr { $$ = $1 / $3; }
    | expr POW expr { $$ = pow($1, $3); }
    | BO expr BC { $$ = $2; }
    | PLUS { $$ = 1; }
    | SUB { $$ = -1; }
    | PLUS expr { $$ = $2; }
    | SUB expr { $$ = $2*-1; }
    ;

trailingEOLS: EOL
	    | EOL trailingEOLS ;// trailing start, require at least one eol
trailingEOL: %empty
	   | EOL trailingEOL 
	   ;

%%

void 
yyerror(YYLTYPE *yylloc, yyscan_t scanner, const char *msg)
{
	fprintf(stderr, "parsing failed at: ");
	YYLOCATION_PRINT(stderr, yylloc);
	die(" because of %s", msg);
}
