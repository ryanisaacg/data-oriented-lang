/*-*-C-*-*/
%{
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
	int line = 0;
%}

%%
[A-Za-z_]+[A-Za-z0-9_]* { yylval.sval = yytext; return WORD;}
[0-9]+ { yylval.ival = atoi(yytext); return INT; }
[ \t] {}
"(" { return LPAREN; }
")" { return RPAREN; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return TIMES; }
"/" { return DIVIDE; }
"%" { return MODULO; }
"**" { return EXP; }
"\n" { line++; return LINE_END; }
"\r" {}
"," { return COMMA; }
%%
void yyerror(char *message) {
	fprintf(stderr, "%s at line %d", message, line);
}
int yywrap(void) {
	return 1;
}
