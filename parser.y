//-*-C-*-
%code requires 
{
	#include "node.h"
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

%union { char* sval; node *nval; int ival; node_type typeval;}


%token <sval> WORD
%token '('
%token ')'
%token COMMA
%token LINE_END
%token <ival> INT
%token PLUS MINUS MODULO
%token TIMES DIVIDE
%token EXP
%token TYPE_TOKEN
%token STRUCT_TOKEN
%token END_TOKEN
%token WHITESPACE

%%
root:
	statement { print_expression($<nval>1);}
	| struct_def {print_expression($<nval>1); }
	| root LINE_END
	| root statement { print_expression($<nval>2); }
	| root struct_def {print_expression($<nval>2); }
statement:
	expression LINE_END
//OPERATORS
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
	| '(' expression ')' {
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
//FUNCTIONS
param_list:
	/*Empty parameter list*/ { $<nval>$ = new_list_node(0); } 
	| expression { node *list = new_list_node(10); add_to_list(list, $<nval>1); $<nval>$ = list; }
	| param_list COMMA expression { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
call:
	expression '(' param_list ')' {
		node *expr = malloc(sizeof(node));
		expr->type = FUNC_CALL;
		expr->data.call.function = $<nval>1;
		expr->data.call.parameters = $<nval>3;
		$<nval>$ = expr; }
//STRUCT
struct_param:
	name type {
		$<nval>$ = new_binary_node(STRUCT_MEMBER, $<nval>1, $<nval>2); }
struct_body:
	/*Empty body allowed, because why not */ {$<nval>$ = new_list_node(0); }
	| struct_param {$<nval>$ = new_list_node(10); add_to_list($<nval>$, $<nval>1); }
	| struct_body LINE_END struct_param { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
struct_def:
	TYPE_TOKEN name STRUCT_TOKEN LINE_END struct_body LINE_END END_TOKEN LINE_END{ $<nval>$ = new_binary_node(STRUCT_DELARATION, $<nval>2, $<nval>5); }
//TYPES
type:
	WORD {
		node *expr = malloc(sizeof(node));
		expr->type = TYPE;
		expr->data.string = strdup($1);
		$<nval>$ = expr;
	}

//SINGLE TOKENS
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

extern FILE* yyin;

int main(void) {
	FILE *input = fopen("current.acc", "r");
	yyin = input;
	yyparse();
	fclose(input);
	return 0;
}
