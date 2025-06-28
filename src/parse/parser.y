%code top {
	int yylex(void);
	void yyerror(const char *);
}
%define parse.trace
%define parse.error detailed
%define api.token.prefix {TOK_}

%code requires {
	%include <math.h>
}

%token
	MIN
	MAX
	ST
	VAR
	COMMENT
	IQLT
	IQGT
	EQ
	END
	EOL
;
%token <double> NUM "number"

%printer { fprintf (yyo, "%g", $$); } <double>;

%left "+" "-" "*" "/" "^";
exp:
	"number"
	| exp "+" exp { $$ = $1 + $3; }
	| exp "-" exp   { $$ = $1 - $3; }
	| exp "*" exp   { $$ = $1 * $3; }
	| exp "/" exp   { $$ = $1 / $3; }
	| "(" exp ")"   { $$ = $2; }
	| exp "^" exp   { $$ = pow($1, $3); }
	| "sqrt(" exp ")" { $$ = sqrt($2); }
