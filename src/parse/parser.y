%code top {
	struct constr;
	struct expr;
}

%define parse.trace
%define parse.error detailed
%define api.token.prefix {TOK_}

%code requires {
	#include <math.h>
	#include "interfaces/highs_c_api.h"
	#include <stddef.h>

	#define MAX_ARRAY_SIZE 100

	typedef double YYSTYPE;
	static HighsInt findIndex(void *mod, const char *text);
	void yyerror(const char *);
	int Index[MAX_ARRAY_SIZE];
	double Val[MAX_ARRAY_SIZE];
	int insert_index = 0;
	void *model = NULL;
}

%token
	MIN
	MAX
	ST
	COMMENT
	LT
	GT
	EQ
	END
	EOL
;

%token <double> NUM "number"
%token <char *> VAR "variable"
%nterm <struct constr> constr
%nterm <struct expr> expr
%nterm <double> coef

%printer { fprintf (yyo, "%g", $$); } <double>;

%%

%start main;

main: MIN obj ST constrs { Highs_changeObjectiveSense(model, kHighsObjSenseMaximize); }
	| MAX obj ST constrs { Highs_changeObjectiveSense(model, kHighsObjSenseMinimize); };

obj: expr { }
   | expr "+" "number" { Highs_changeObjectiveOffset(model, $3); 
	for (size_t i = 0; i < $1.n; i++)
		Highs_changeColCost(model, $1.i[i], $1.c[i]);
   };

constrs:
       %empty
       | constr EOL constrs { Highs_addRow(model, $1.lower, $1.upper, $1.numNz, $1.Index, $1.Val); }
constr:
      expr LT "number" { 
      		$$.lower = Highs_getInfinity(model); 
		$$.upper = $3; 
		$$.Index = $1.i; 
		$$.Val = $1.c; 
		$$.numNz = $1.n;
	}
      | expr GT "number" { 
      		$$.upper = Highs_getInfinity(model); 
      		$$.lower = $3; 
		$$.Index = $1.i; 
		$$.Val = $1.c; 
		$$.numNz = $1.n;
	}
      | expr EQ "number" { 
      		$$.lower = $3; 
      		$$.upper = $3; 
		$$.Index = $1.i; 
		$$.Val = $1.c; 
		$$.numNz = $1.n;
	};

expr:
    %empty {}
    | coef VAR expr { 
		$$.i = Index; 
		$$.c = Val; 
		$$.i[$$.n] = findIndex(model, $2);
		$$.c[$$.n++] = $1;
	}

%left "+" "-" "*" "/" "^";
coef:
	"number"
	| coef "+" coef { $$ = $1 + $3; }
	| coef "-" coef   { $$ = $1 - $3; }
	| coef "*" coef   { $$ = $1 * $3; }
	| coef "/" coef   { $$ = $1 / $3; }
	| "(" coef ")"   { $$ = $2; }
	| coef "^" coef   { $$ = pow($1, $3); }
	| "sqrt(" coef ")" { $$ = sqrt($2); };

%%

struct constr {
	double *Index, *Val, lower, upper;
	int numNz;
};
struct expr {
	double *c;
	HighsInt *i;
	size_t n;
};

/*
  return the column index of a variable name.
  add it if it is not present
*/
static HighsInt
findIndex(void *mod, const char *text)
{
  HighsInt index;
  if (Highs_getColByName(mod, text, &index) == kHighsStatusError) {// returns Error when col does not exist
    #ifdef DEBUG
    printf("adding %s to index\n", text);
    #endif
    index = Highs_getNumCol(mod);
    Highs_addVar(mod, -Highs_getInfinity(mod), Highs_getInfinity(mod));
    Highs_passColName(mod, index, text);
  }
  return (size_t) index;
}

int main() {
	return yyparse();
}
