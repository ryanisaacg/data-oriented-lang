/*-*-C-*-*/
%{
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
%}
%%
[A-Za-z_]+[A-Za-z0-9_]* { yylval.sval = yytext; return WORD;}
[0-9]+ { yylval.ival = atoi(yytext); return INT; }
[ \t] {}
"(" { return LPAREN; }
")" { return RPAREN; }
"\n" { return LINE_END; }
%%
void yyerror(char *message) {
	fprintf(stderr, "%s", message);
}
int yywrap(void) {
	return 1;
}
