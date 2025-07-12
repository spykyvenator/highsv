%option nodefault noinput nounput noyywrap

%{
#include <stdlib.h>
#include "parser.h"
%}

%%

(?i:min) return HIGHSV_MIN;
(?i:max) return HIGHSV_MAX;
(?i:st)|(?i:subject\ to) return HIGHSV_ST;
[0-9]+   {
  double n = strtod (yytext, NULL);
  yylval->HIGHSV_NUM = (int) n;
  return HIGHSV_NUM;
}
"\n" return HIGHSV_EOL;
<*>. {}

%%
