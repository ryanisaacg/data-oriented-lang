/*-*-C-*-*/
%{
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
%}

%option yylineno

%%
"type" { return TYPE_TOKEN; }
"struct" { return STRUCT_TOKEN; }
"end" { return END_TOKEN; }
[A-Za-z_]+[A-Za-z0-9_]* { yylval.sval = yytext; return WORD;}
[0-9]+ { yylval.ival = atoi(yytext); return INT; }
[ \t] {}
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return TIMES; }
"/" { return DIVIDE; }
"%" { return MODULO; }
"**" { return EXP; }
"\n" { return LINE_END; }
"\r" {}
. { return yytext[0]; }
%%
void yyerror(char *message) {
	fprintf(stderr, "%s at line %d", message, yylineno);
}
int yywrap(void) {
	return 1;
}
