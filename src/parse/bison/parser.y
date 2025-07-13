%define parse.trace
%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}

%code top {
	#include "interfaces/highs_c_api.h"
	#include <stddef.h>
	void *model;
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
;

%token <double> NUM "number"
%token <char *> VAR "var"

%printer { fprintf (yyo, "%f", $$); } <double>
%left "+" "-"
%left "*" "/"

%%

%start input;

input: %empty
     | MAX obj EOL st trailingEOL { puts("max"); }
     | MIN obj EOL st trailingEOL { puts("min"); }
     ;

st: ST { puts("st"); };
 
obj: %empty
   | obj "number" VAR { printf("%s: %f\n", $3, $2); };

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
