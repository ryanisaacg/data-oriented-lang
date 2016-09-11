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
	node r = {.data = {.unary = NULL}, .type= ROOT};
	node *root = &r;
%}

%union { char* sval; node *nval; int ival; node_type typeval}


%token <sval> WORD
%token LPAREN
%token RPAREN
%token LINE_END
%token <ival> INT
%token PLUS
%token MINUS
%token TIMES
%token DIVIDE
%token MODULO
%token EXP
%%
root:
	statement { print_expression($<nval>1); }
	| root statement { print_expression($<nval>2); }
statement:
	expression LINE_END
binary_operator:
	PLUS { $<typeval>$ = OP_ADD; }
	| MINUS { $<typeval>$ = OP_SUB; }
	| TIMES { $<typeval>$ = OP_MULT; }
	| DIVIDE { $<typeval>$ = OP_DIV; }
	| MODULO { $<typeval>$ = OP_MOD; }
	| EXP { $<typeval>$ = OP_EXP; }
unary_operator:
	MINUS {$<typeval>$ = OP_SUB; }
expression:
	call
	| name
	| number
	| LPAREN expression RPAREN {
		$<nval>$ = $<nval>2; }
	| expression binary_operator expression {
		node *expr = new_binary_node($<typeval>2, $<nval>1, $<nval>3);
		$<nval>$ = expr;
	}
	| unary_operator expression {
		node *expr = malloc(sizeof(node));
		expr->type = OP_NEGATIVE;
		expr->data.unary = $<nval>2;
		$<nval>$ = expr; }
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
		expr->data.string = strdup($1);
		$<nval>$ = expr; }
number:
	INT {
		node *expr = malloc(sizeof(node));
		expr->type = NUM;
		expr->data.integer = $1;
		$<nval>$ = expr; }
%%


int main(void) {
	yyparse();
	return 0;
}
