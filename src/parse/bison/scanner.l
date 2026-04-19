%option nodefault noinput nounput noyywrap
FLOAT (?x:[-+]?[\ ]?([0-9]*[\.])?[0-9]+)

%{
#include <stdlib.h>
#include "parser.h"
#include <ctype.h>

double highsv_atof(char *c);
%}

%%

(?i:min) return HIGHSV_MIN;
(?i:max) return HIGHSV_MAX;
(?i:st)|(?i:subject\ to) return HIGHSV_ST;
{FLOAT} {
  double n = highsv_atof(yytext);
  yylval->HIGHSV_NUM = (int) n;
  return HIGHSV_NUM;
}
[a-zA-Z][0-9a-zA-Z|,|/]* { yylval->HIGHSV_VAR = strdup(yytext); return HIGHSV_VAR; }// maybe we can pass it with passColName and use the col index
"<" return HIGHSV_LESS;
">" return HIGHSV_MORE;
"=" return HIGHSV_EQUAL;
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
