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
%token LINE_END
%token <ival> INT
%token TYPE_TOKEN
%token STRUCT_TOKEN
%token END_TOKEN
%token WHITESPACE


%left '+' '-' '%'
%left '*' '/'
%right '^'
%left UMINUS

%nonassoc GROUP
%nonassoc CALL

%%
root:
	statement { print_expression($<nval>1);}
	| struct_def {print_expression($<nval>1); }
	| root LINE_END
	| root statement { print_expression($<nval>2); }
	| root struct_def {print_expression($<nval>2); }
statement:
	expression LINE_END

//FUNCTIONS
param_list:
	/*Empty parameter list*/ { $<nval>$ = new_list_node(0); } 
	| expression { node *list = new_list_node(10); add_to_list(list, $<nval>1); $<nval>$ = list; }
	| param_list ',' expression { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }

expression:
	name
	| number
	| '(' expression ')' %prec CALL {
		$<nval>$ = $<nval>2; }
	| expression '+' expression { $<nval>$ = new_binary_node(OP_ADD, $<nval>1, $<nval>3); }
	| expression '-' expression { $<nval>$ = new_binary_node(OP_SUB, $<nval>1, $<nval>3); }
	| expression '*' expression { $<nval>$ = new_binary_node(OP_MULT, $<nval>1, $<nval>3); }
	| expression '/' expression { $<nval>$ = new_binary_node(OP_DIV, $<nval>1, $<nval>3); }
	| expression '%' expression { $<nval>$ = new_binary_node(OP_MOD, $<nval>1, $<nval>3); }
	| '-' expression %prec UMINUS { $<nval>$ = new_unary_node(OP_NEGATIVE, $<nval>2); }
	| expression '^' expression { $<nval>$ = new_binary_node(OP_EXP, $<nval>1, $<nval>3); }
	| name '(' param_list ')' %prec GROUP { $<nval>$ = new_binary_node(FUNC_CALL, $<nval>1, $<nval>3); } 

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
