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
"func" { return FUNC_TOKEN; }
"end" { return END_TOKEN; }
"if" { return IF_TOKEN; }
"while" { return WHILE_TOKEN; }
"return" { return RETURN_TOKEN; }
"export" { return EXPORT_TOKEN; }
"import" { return IMPORT_TOKEN; }
"var" { return VAR_TOKEN; }
[A-Za-z_]+[A-Za-z0-9_]* { yylval.sval = yytext; return WORD;}
[0-9]+ { yylval.ival = atoi(yytext); return INT; }
[ \t] {}
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
