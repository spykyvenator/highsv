%option nodefault noinput nounput noyywrap
FLOAT (?x:[\ ]?([0-9]*[\.])?[0-9]+)

%{
#include <stdlib.h>
#include "parser.h"
#include <ctype.h>

double highsv_atof(char *c);
%}

%%


!.*\n return HIGHSV_EOL;
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
"\n" return HIGHSV_EOL;
<*>. {}

%%

double
highsv_atof(char *c) 
{
	double res = 0;
	char sign = 1;
	while (isspace(*c)) c++;
	while (*c != '\0') { switch (*c) {
			case '+': sign = 1; break;
			case '-': sign = -1; break;
			case '\t':
			case '\v':
			case '\f':
			case '\n':
			case '\r':
			case ' ':
				break;
			default:
				if (*c >= '0' && *c <= '9'){
					res *= 10;
					res += *c - '0';
				} else {
					return res*sign;
				}
			} 
			c++;
		}
	return res*sign;
}
