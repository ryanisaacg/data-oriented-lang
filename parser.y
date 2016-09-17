//-*-C-*-
%code requires
{
	#include "node.h"
}

%{
	#include "analyzer.h"
	#include "output.h"
	#include "printing.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "yacc.tab.h"

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
%token ELSE_TOKEN
%token ELSE_IF_TOKEN
%token RETURN_TOKEN
%token EXPORT_TOKEN
%token IMPORT_TOKEN
%token VAR_TOKEN
%token TRUE_TOKEN
%token FALSE_TOKEN

%left '='
%left '+' '-' '%'
%left '*' '/'
%right '^'
%nonassoc UMINUS
%left '.'

%parse-param {node *root_node}

%%
root:
	/* Empty root */
	| root LINE_END
	| root struct_def { add_to_list(root_node->data.root.struct_list, $<nval>2); }
	| root function_def { add_to_list(root_node->data.root.func_list, $<nval>2); }
	| root external { add_to_list(root_node->data.root.ext_list, $<nval>2); }
	| root statement { add_to_list(root_node->data.root.main_list, $<nval>2); }
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
	| boolean
	| '(' expression ')' {
		$<nval>$ = $<nval>2; }
	| expression '+' expression { $<nval>$ = new_binary_node(OP_ADD, $<nval>1, $<nval>3); }
	| expression '-' expression { $<nval>$ = new_binary_node(OP_SUB, $<nval>1, $<nval>3); }
	| expression '*' expression { $<nval>$ = new_binary_node(OP_MULT, $<nval>1, $<nval>3); }
	| expression '/' expression { $<nval>$ = new_binary_node(OP_DIV, $<nval>1, $<nval>3); }
	| expression '%' expression { $<nval>$ = new_binary_node(OP_MOD, $<nval>1, $<nval>3); }
	| expression '=' expression { $<nval>$ = new_binary_node(OP_ASSIGN, $<nval>1, $<nval>3); }
	| expression '.' expression { $<nval>$ = new_binary_node(OP_MEMBER, $<nval>1, $<nval>3); }
	| '-' expression %prec UMINUS { $<nval>$ = new_unary_node(OP_NEGATIVE, $<nval>2); }
	| expression '^' expression { $<nval>$ = new_binary_node(OP_EXP, $<nval>1, $<nval>3); }
	| name '(' param_list ')' { $<nval>$ = new_binary_node(FUNC_CALL, $<nval>1, $<nval>3); }
	| type '{' param_list '}' { $<nval>$ = new_binary_node(TYPE_LITERAL, $<nval>1, $<nval>3); }
	| '[' param_list ']' {$<nval>$ = new_unary_node(ARRAY_LITERAL, $<nval>2); }
declare_list:
	name { $<nval>$ = new_list_node(10); add_to_list($<nval>$, $<nval>1); }
	| name '=' expression { $<nval>$ = new_list_node(10); add_to_list($<nval>$, new_binary_node(OP_ASSIGN, $<nval>1, $<nval>3)); }
	| declare_list ',' name { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
	| declare_list ',' name '=' expression { add_to_list($<nval>1, new_binary_node(OP_ASSIGN, $<nval>3, $<nval>5)); $<nval>$ = $<nval>1;}
variable_declaration:
	VAR_TOKEN type declare_list { $<nval>$ = new_binary_node(OP_INIT, $<nval>2, $<nval>3); }
//STRUCT
name_type_pair:
	type name {
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
	| variable_declaration LINE_END
	| RETURN_TOKEN expression LINE_END { $<nval>$ = new_unary_node(RETURN, $<nval>2); }
//CONTROL STRUCTURES
else_list:
	| ELSE_IF_TOKEN expression LINE_END block { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, NULL); }
	| else_list else_alone { $<nval>$->data.ternary[2] = $<nval>2; }
	| else_list ELSE_IF_TOKEN expression LINE_END block { $<nval>$->data.ternary[2] = new_ternary_node(IF, $<nval>3, $<nval>5, NULL); }
else_alone:
	ELSE_TOKEN block { $<nval>$ = $<nval>2; }
elses:
	else_list
	| else_alone
control:
	IF_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, NULL); }
	| WHILE_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_binary_node(WHILE, $<nval>2, $<nval>4);	}
	| IF_TOKEN expression LINE_END block elses END_TOKEN { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, $<nval>5); }
//TYPES
type:
	WORD {
		node *expr = malloc(sizeof(node));
		expr->type = TYPE;
		expr->data.string = $1;
		$<nval>$ = expr;
	}
	| '[' type ']' { $<nval>$ = new_unary_node(ARRAY_OF, $<nval>2);	}

//SINGLE TOKENS
name:
	WORD {
		node *expr = malloc(sizeof(node));
		expr->type = NAME;
		expr->data.string = $1;
		$<nval>$ = expr; }
number:
	INT {
		node *expr = malloc(sizeof(node));
		expr->type = NUM;
		expr->data.integer = $1;
		$<nval>$ = expr; }
boolean:
	TRUE_TOKEN {
		node *expr = malloc(sizeof(node));
		expr->type = VALUE_TRUE;
		$<nval>$ = expr; }
	| FALSE_TOKEN {
		node *expr = malloc(sizeof(node));
		expr->type = VALUE_FALSE;
		$<nval>$ = expr; }
%%

extern FILE* yyin;

int main(void) {
	FILE *input = fopen("current.acc", "r");
	yyin = input;
	node root;
	root.type = ROOT;
	root.data.root.ext_list = new_list_node(10);
	root.data.root.struct_list = new_list_node(10);
	root.data.root.func_list = new_list_node(10);
	root.data.root.main_list = new_list_node(10);
	yyparse(&root);
	fclose(input);
	c_ast_node result = analyze(root.data.root);
	FILE *out = fopen("build/output.c", "w");
	c_write(out, result);
	fclose(out);
	print_expression(&root);
	return 0;
}
