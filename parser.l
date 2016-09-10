/*-*-C-*-*/
%{
	#include "yacc.tab.h"
%}
%%
[A-Za-z0-9_]+ { yylval.sval = yytext; return WORD;}
[ \t] {}

%%
void yyerror(char *message) {
	fprintf(stderr, "%s", message);
}
int yywrap(void) {
	return 1;
}
