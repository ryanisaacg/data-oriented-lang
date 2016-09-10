//-*-C-*-
%{
	#include "yacc.tab.h"
	#include <stdio.h>
%}

%union { char* sval; }

%token <sval> WORD

%%

list:
	WORD {printf("%s", $1); }
	| list WORD { printf("%s", $2); }

%%


int main(void) {
	yyparse();
	return 0;
}
