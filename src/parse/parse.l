fpc [0-9]*[\.,]?[0-9]*

%{
	float readEntry(char *text);
	int i = 0;
	float values[32];
	float sign = 1;
%}

%%

{fpc}\ *[a-zA-Z][0-9a-zA-Z]* {values[i++] = readEntry(yytext);}

\+ {sign = 1;}
\- {sign = -1;}

"> "{fpc} {puts("eol found");}
"< "{fpc} {puts("eol found");}
"= "{fpc} {puts("eol found");}
"<= "{fpc} {puts("eol found");}
">= "{fpc} {puts("eol found");}

%%

#include <stdlib.h>

float
readEntry(char *text) 
{
	char *endptr;
	float value = strtof(text, &endptr);
	if (*text != '0' && value == 0)
		value = 1;
	value *= sign;
	while (*endptr == ' ') endptr++;
	printf("value: %f: from %s", value, endptr);
	return value;
}

int
main () {
	yylex();
	printf("index: %d\n", i);
	for (int j = 0; j < i; j++)
		printf("value: %f\n", values[j]);
}
