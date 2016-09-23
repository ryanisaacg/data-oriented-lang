/*-*-C-*-*/
%{
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
%}

%option yylineno

%%
"type" { return TYPE_TOKEN; }
"struct" { return STRUCT_TOKEN; }
"func" { return FUNC_TOKEN; }
"end" { return END_TOKEN; }
"if" { return IF_TOKEN; }
"while" { return WHILE_TOKEN; }
"else" { return ELSE_TOKEN; }
"elseif" { return ELSE_IF_TOKEN; }
"return" { return RETURN_TOKEN; }
"export" { return EXPORT_TOKEN; }
"import" { return IMPORT_TOKEN; }
"var" { return VAR_TOKEN; }
"true" { return TRUE_TOKEN; }
"false" { return FALSE_TOKEN; }
"==" { return EQ_TOKEN; }
"!=" { return NEQ_TOKEN; }
"<=" { return LEQ_TOKEN; }
">=" { return GEQ_TOKEN; }
"cextern" { return CEXTERN_TOKEN; }
"cimport" { return CIMPORT_TOKEN; }
"clink" { return CLINK_TOKEN; }
"new" { return NEW_TOKEN; }

[A-Za-z_]+[A-Za-z0-9_]* {
	char *text = strdup(yytext);
	int len = strlen(text);
	char last = text[len - 1];
	if(!(last >= 'A' && last <= 'Z' || last >= 'a' && last <= 'z' || last >= '0' && last <= '9' || last == '_'))
		text[len - 1] = '\0';
	yylval.sval = text;
	return WORD;}
[0-9]+ { yylval.ival = atoi(yytext); return INT; }
[ \t] {}
["]([^"]|\\\")*["] { yylval.sval = strdup(yytext); return STRING_TOKEN; }
"#".*"\n" {} /*Comments*/
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
