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

//Values
%token <sval> WORD
%token <ival> INT
//Special placeholders
%token WHITESPACE
%token LINE_END
//Reserved words
%token TYPE_TOKEN
%token STRUCT_TOKEN
%token END_TOKEN
%token FUNC_TOKEN
%token IF_TOKEN
%token WHILE_TOKEN
%token RETURN_TOKEN 
%token EXPORT_TOKEN
%token IMPORT_TOKEN

%left '=' 
%left '+' '-' '%'
%left '*' '/'
%right '^'
%nonassoc UMINUS

%%
root:
	/* Empty root */
	| root LINE_END
	| root struct_def { print_expression($<nval>2); }
	| root function_def {print_expression($<nval>2); }
	| root external { print_expression($<nval>2); }
//EXPORTS / IMPORTS
path:
	name { $<nval>$ = new_list_node(10); add_to_list($<nval>$, $<nval>1); }
	| path '.' name {add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1;}
external:
	EXPORT_TOKEN path { $<nval>$ = new_unary_node(EXPORT, $<nval>2); }
	| IMPORT_TOKEN path { $<nval>$ = new_unary_node(IMPORT, $<nval>2); }
	
//FUNCTIONS
param_list:
	/*Empty parameter list*/ { $<nval>$ = new_list_node(0); } 
	| expression { node *list = new_list_node(10); add_to_list(list, $<nval>1); $<nval>$ = list; }
	| param_list ',' expression { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }

expression:
	name
	| number
	| '(' expression ')' {
		$<nval>$ = $<nval>2; }
	| expression '+' expression { $<nval>$ = new_binary_node(OP_ADD, $<nval>1, $<nval>3); }
	| expression '-' expression { $<nval>$ = new_binary_node(OP_SUB, $<nval>1, $<nval>3); }
	| expression '*' expression { $<nval>$ = new_binary_node(OP_MULT, $<nval>1, $<nval>3); }
	| expression '/' expression { $<nval>$ = new_binary_node(OP_DIV, $<nval>1, $<nval>3); }
	| expression '%' expression { $<nval>$ = new_binary_node(OP_MOD, $<nval>1, $<nval>3); }
	| expression '=' expression { $<nval>$ = new_binary_node(OP_ASSIGN, $<nval>1, $<nval>3); }
	| '-' expression %prec UMINUS { $<nval>$ = new_unary_node(OP_NEGATIVE, $<nval>2); }
	| expression '^' expression { $<nval>$ = new_binary_node(OP_EXP, $<nval>1, $<nval>3); }
	| name '(' param_list ')' { $<nval>$ = new_binary_node(FUNC_CALL, $<nval>1, $<nval>3); } 

//STRUCT
name_type_pair:
	name type {
		$<nval>$ = new_binary_node(STRUCT_MEMBER, $<nval>1, $<nval>2); }
struct_body:
	/*Empty body allowed, because why not */ {$<nval>$ = new_list_node(0); }
	| name_type_pair {$<nval>$ = new_list_node(10); add_to_list($<nval>$, $<nval>1); }
	| struct_body LINE_END name_type_pair { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
struct_def:
	TYPE_TOKEN name STRUCT_TOKEN LINE_END struct_body LINE_END END_TOKEN { $<nval>$ = new_binary_node(STRUCT_DELARATION, $<nval>2, $<nval>5); }
//FUNCTION DECLARATIONS
function_params:
	/*Empty body allowed, because why not */ {$<nval>$ = new_list_node(0); }
	| name_type_pair {$<nval>$ = new_list_node(10); add_to_list($<nval>$, $<nval>1); }
	| function_params ',' name_type_pair { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
block:
	/*Empty block*/ { $<nval>$ = new_list_node(10); }
	| block statement { add_to_list($<nval>1, $<nval>2); $<nval>$ = $<nval>1; }
function_def:
	FUNC_TOKEN name '(' function_params ')' type LINE_END block END_TOKEN {
		node *func = malloc(sizeof(node));
		func->type = FUNCTION_DECLARATION;
		func->data.func.name = $<nval>2;
		func->data.func.return_type = $<nval>6;
		func->data.func.params = $<nval>4;
		func->data.func.body = $<nval>8;
		$<nval>$ = func; }
statement:
	expression LINE_END
	| control LINE_END
	| RETURN_TOKEN expression LINE_END { $<nval>$ = new_unary_node(RETURN, $<nval>2); }
//CONTROL STRUCTURES
control:
	IF_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_binary_node(IF, $<nval>2, $<nval>4); }
	| WHILE_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_binary_node(WHILE, $<nval>2, $<nval>4);	}
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
