%code top {
	int yylex(void);
	void yyerror(const char *);
}
%define parse.trace
%define parse.error detailed
%define api.token.prefix {TOK_}

%code requires {
	%include <math.h>
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
%nterm <double> coef

%printer { fprintf (yyo, "%g", $$); } <double>;

%%
%start main

main:
	MIN obj ST constr { 
		Highs_changeObjectiveSense(model, kHighsObjSenseMaximize); }
	| MAX obj ST constr {
		Highs_changeObjectiveSense(model, kHighsObjSenseMinimize); }
obj: %empty 

constr:
      expr LT "number" {
	      Highs_addRow(model, Highs_getInfinity(mod), $3, numNz, $1.Index, $1.Val); }
      | expr GT "number" {
	      Highs_addRow(model, $3, Highs_getInfinity(mod), numNz, $1.Index, $1.Val); }
      | expr EQ "number" {
	      Highs_addRow(model, $3, $3, numNz, $1.Index, $1.Val); }

expr:
    %empty
    | coef VAR expr
	
%left "+" "-" "*" "/" "^";
coef:
	"number"
	| coef "+" coef { $$ = $1 + $3; }
	| coef "-" coef   { $$ = $1 - $3; }
	| coef "*" coef   { $$ = $1 * $3; }
	| coef "/" coef   { $$ = $1 / $3; }
	| "(" coef ")"   { $$ = $2; }
	| coef "^" coef   { $$ = pow($1, $3); }
	| "sqrt(" coef ")" { $$ = sqrt($2); }

%%
