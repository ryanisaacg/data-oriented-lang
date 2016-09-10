//-*-C-*-
%{
	#include "expression.h"
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
	node r = {.data = {.root= {.body = NULL}}, .type= ROOT};
	node *root = &r;
%}

%type node_type node*
%union { char* sval; node_type nval; }

%token <sval> WORD

%%
root:
	statement { execute($<nval>1); }
statement:
	call {$<nval>$ = $<nval>1;}
call:
	name '(' ')' {
		node *expr = malloc(sizeof(node));
		expr->type = FUNC_CALL;
		expr->data.call.num_params = 0;
		expr->data.call.params = NULL;
		expr->data.call.function = $<nval>1;
		$<nval>$ = expr;
	}
name:
	WORD { 
		node *expr = malloc(sizeof(node));
		expr->type = NAME;
		expr->data.string.str = $1;
		$<nval>$ = expr; }

%%


int main(void) {
	yyparse();
	return 0;
}
