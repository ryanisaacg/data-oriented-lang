//-*-C-*-
%code requires 
{
	#include "expression.h"
}

%{
	#include "printing.h"
	#include "yacc.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	node r = {.data = {.root= {.body = NULL}}, .type= ROOT};
	node *root = &r;
%}

%union { char* sval; node *nval; int ival; }


%token <sval> WORD
%token LPAREN
%token RPAREN
%token LINE_END
%token <ival> INT
%token PLUS
%token MINUS
%%
root:
	statement { print_expression($<nval>1); }
statement:
	expression LINE_END
expression:
	call
	| name
	| number
	| expression PLUS expression {
		node *expr = malloc(sizeof(node));
		expr->type = OP_ADD;
		expr->data.binary.left = $<nval>1;
		expr->data.binary.right = $<nval>2;
		$<nval>$ = expr;
	}
	| expression MINUS expression {
		node *expr = malloc(sizeof(node));
		expr->type = OP_SUB;
		expr->data.binary.left = $<nval>1;
		expr->data.binary.right = $<nval>2;
		$<nval>$ = expr;
	}
call:
	expression LPAREN RPAREN {
		node *expr = malloc(sizeof(node));
		expr->type = FUNC_CALL;
		expr->data.call.num_params = 0;
		expr->data.call.params = NULL;
		expr->data.call.function = $<nval>1;
		$<nval>$ = expr; }
	| expression LPAREN expression RPAREN {
		node *expr = malloc(sizeof(node));
		expr->type = FUNC_CALL;
		expr->data.call.num_params = 1;
		expr->data.call.params = malloc(sizeof(node*));
		expr->data.call.params[0] = $<nval>3;
		expr->data.call.function = $<nval>1;
		$<nval>$ = expr; }
name:
	WORD { 
		node *expr = malloc(sizeof(node));
		expr->type = NAME;
		expr->data.string.str = strdup($1);
		$<nval>$ = expr; }
number:
	INT {
		node *expr = malloc(sizeof(node));
		expr->type = NUM;
		expr->data.integer.value = $1;
		$<nval>$ = expr; }
%%


int main(void) {
	yyparse();
	return 0;
}
