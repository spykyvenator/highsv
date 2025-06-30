%option nodefault noinput nounput noyywrap reentrant noyywrap

%{
#include "parser.tab.h"
%}

FLOAT [0-9]*[\.]?[0-9]*

%%

!.*\n return TOK_COMMENT;
(?i:max) return TOK_MAX;
(?i:min) return TOK_MIN;
(?i:st)|(?i:subject\ to) return TOK_ST;
">"|">="|"=>" return TOK_GT;
"<"|"<="|"=<" return TOK_LT;
"="|"==" return TOK_EQ;
{FLOAT} {
	errno = 0;
	double n = strtod(yytext, NULL);
	yylval = n;
	return TOK_NUM;
}
^(?i:end) return TOK_END;
[a-zA-Z][0-9a-zA-Z|,|/]* return TOK_VAR;
\n return TOK_EOL;
