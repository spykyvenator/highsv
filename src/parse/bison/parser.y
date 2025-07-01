%define parse.trace
%define parse.error detailed
%define api.token.prefix {HIGHSV_}
%define api.pure full
%define api.value.type union
%define api.header.include {"parser.h"}

%code top {
	#include "interfaces/highs_c_api.h"
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
;

%token <double> NUM "number"

%left "+" "-"
%left "*" "/"

%%
input:
     MAX ST { puts("max"); }
     | MIN ST { puts("min"); }
     ;
%%

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
