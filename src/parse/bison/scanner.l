%option nodefault noinput nounput noyywrap reentrant bison-bridge yylineno
FLOAT (?x:[\ ]?([0-9]*[\.])?[0-9]+)

%{
#include <stdlib.h>
#include "parser.h"
#include <ctype.h>

#define YY_USER_ACTION                          \
  yylloc->last_column += (int) yyleng;

#define LOCATION_STEP()                         \
    yylloc->first_line = yylloc->last_line;     \
    yylloc->first_column = yylloc->last_column; 
%}

%%

%{
LOCATION_STEP();
%}

!.*\n {}
^(?i:end) return 0;
(?i:min) return HIGHSV_MIN;
(?i:max) return HIGHSV_MAX;
(?i:st)|(?i:subject\ to) return HIGHSV_ST;
{FLOAT} {
  double n = atof(yytext);
  yylval->HIGHSV_NUM = n;
  return HIGHSV_NUM;
}
"+" return HIGHSV_PLUS;
"-" return HIGHSV_SUB;
"*" return HIGHSV_MULT;
"/" return HIGHSV_DIV;
"^" return HIGHSV_POW;
"(" return HIGHSV_BO;
")" return HIGHSV_BC;
[a-zA-Z][0-9a-zA-Z|,|/]* { yylval->HIGHSV_VAR = strdup(yytext); return HIGHSV_VAR; }// maybe we can pass it with passColName and use the col index
"<"|"<="|"=<" return HIGHSV_LESS;
">"|">="|"=>" return HIGHSV_MORE;
"="|"==" return HIGHSV_EQUAL;
"\n" { yylloc->last_line++; yylloc->last_column = 1; return HIGHSV_EOL; }
<*>. {}
