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
	VAR
	COMMENT
	LT
	GT
	EQ
	END
	EOL
;
%token <double> NUM "number"
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
      expr LT expr
      | expr GT expr
      | expr EQ expr

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
