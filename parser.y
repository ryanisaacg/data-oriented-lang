//-*-C-*-
%code requires
{
	#include "node.h"
}

%{
	#include "analyzer.h"
	#include "output.h"
	#include "printing.h"
	#include <stdlib.h>
	#include <string.h>
	#include "yacc.tab.h"

	node r = {.data = {.unary = NULL}, .type= ROOT};
	node *root = &r;
	extern int yylineno;
%}

%union { char* sval; node *nval; int ival; node_type typeval;}

//Values
%token <sval> WORD
%token <sval> STRING_TOKEN
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
%token EQ_TOKEN
%token NEQ_TOKEN
%token GEQ_TOKEN
%token LEQ_TOKEN
%token CEXTERN_TOKEN
%token CIMPORT_TOKEN
%token CLINK_TOKEN
%token NEW_TOKEN
%token NS_TOKEN

%left EQ_TOKEN NEQ_TOKEN GEQ_TOKEN LEQ_TOKEN '<' '>'
%left '='
%left '+' '-' '%'
%left '*' '/'
%right '^'
%nonassoc UMINUS '!'
%nonassoc '$' '@'
%left '.'
%nonassoc NEW_TOKEN
%left NS_TOKEN

%parse-param {node *root_node}
%parse-param {char *filename}
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
	name { $<nval>$ = new_list_node(10, (origin){filename, yylineno}); add_to_list($<nval>$, $<nval>1); }
	| path '.' name {add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1;}
cimport:
	CIMPORT_TOKEN name { $<nval>$ = new_binary_node(C_IMPORT, $<nval>2, NULL, (origin){filename, yylineno}); }
	| CIMPORT_TOKEN name string { $<nval>$ = new_binary_node(C_IMPORT, $<nval>2, $<nval>3, (origin){filename, yylineno}); }
clink:
	CLINK_TOKEN string { $<nval>$ = new_binary_node(C_LINK, $<nval>2, new_list_node(10, (origin){filename, yylineno}), (origin){filename, yylineno}); }
	| clink string {add_to_list($<nval>1->data.binary[1], $<nval>2); $<nval>$ = $<nval>1; }
external:
	EXPORT_TOKEN path { $<nval>$ = new_unary_node(EXPORT, $<nval>2, (origin){filename, yylineno}); }
	| IMPORT_TOKEN path { $<nval>$ = new_unary_node(IMPORT, $<nval>2, (origin){filename, yylineno}); }
	| CEXTERN_TOKEN name { $<nval>$ = new_unary_node(C_EXTERN, $<nval>2, (origin){filename, yylineno}); }
	| cimport
	| clink
//FUNCTIONS
param_list:
	/*Empty parameter list*/ { $<nval>$ = new_list_node(0, (origin){filename, yylineno}); }
	| expression { node *list = new_list_node(10, (origin){filename, yylineno}); add_to_list(list, $<nval>1); $<nval>$ = list; }
	| param_list ',' expression { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }

expression:
	name
	| number
	| boolean
	| string
	| '(' expression ')' {
		$<nval>$ = $<nval>2; }
	| name NS_TOKEN expression { $<nval>$ = $<nval>3; $<nval>$->ns = $<nval>1; }
	| expression '+' expression { $<nval>$ = new_binary_node(OP_ADD, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '-' expression { $<nval>$ = new_binary_node(OP_SUB, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '*' expression { $<nval>$ = new_binary_node(OP_MULT, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '/' expression { $<nval>$ = new_binary_node(OP_DIV, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '%' expression { $<nval>$ = new_binary_node(OP_MOD, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '=' expression { $<nval>$ = new_binary_node(OP_ASSIGN, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '.' expression { $<nval>$ = new_binary_node(OP_MEMBER, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '>' expression { $<nval>$ = new_binary_node(OP_GREATER, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression '<' expression { $<nval>$ = new_binary_node(OP_LESS, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression EQ_TOKEN expression { $<nval>$ = new_binary_node(OP_EQUAL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression NEQ_TOKEN expression { $<nval>$ = new_binary_node(OP_NOT_EQUAL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression GEQ_TOKEN expression { $<nval>$ = new_binary_node(OP_GREATER_EQUAL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| expression LEQ_TOKEN expression { $<nval>$ = new_binary_node(OP_LESS_EQUAL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| '-' expression %prec UMINUS { $<nval>$ = new_unary_node(OP_NEGATIVE, $<nval>2, (origin){filename, yylineno}); }
	| '!' expression %prec '!' { $<nval>$ = new_unary_node(OP_BOOL_NOT, $<nval>2, (origin){filename, yylineno}); }
	| '$' expression { $<nval>$ = new_unary_node(OP_DEREF, $<nval>2, (origin){filename, yylineno}); }
	| '@' expression { $<nval>$ = new_unary_node(OP_GETREF, $<nval>2, (origin){filename, yylineno}); }
	| NEW_TOKEN expression { $<nval>$ = new_unary_node(HEAP_INIT, $<nval>2, (origin){filename, yylineno}); }
	| expression '^' expression { $<nval>$ = new_binary_node(OP_EXP, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| name '(' param_list ')' { $<nval>$ = new_binary_node(FUNC_CALL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| type '{' param_list '}' { $<nval>$ = new_binary_node(TYPE_LITERAL, $<nval>1, $<nval>3, (origin){filename, yylineno}); }
	| '[' param_list ']' {$<nval>$ = new_unary_node(ARRAY_LITERAL, $<nval>2, (origin){filename, yylineno}); }
declare_list:
	name { $<nval>$ = new_list_node(10, (origin){filename, yylineno}); add_to_list($<nval>$, $<nval>1); }
	| name '=' expression { $<nval>$ = new_list_node(10, (origin){filename, yylineno}); add_to_list($<nval>$, new_binary_node(OP_ASSIGN, $<nval>1, $<nval>3, (origin){filename, yylineno})); }
	| declare_list ',' name { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
	| declare_list ',' name '=' expression { add_to_list($<nval>1, new_binary_node(OP_ASSIGN, $<nval>3, $<nval>5, (origin){filename, yylineno})); $<nval>$ = $<nval>1;}
variable_declaration:
	type declare_list { $<nval>$ = new_binary_node(OP_INIT, $<nval>1, $<nval>2, (origin){filename, yylineno}); }
//STRUCT
name_type_pair:
	type name {
		$<nval>$ = new_binary_node(STRUCT_MEMBER, $<nval>1, $<nval>2, (origin){filename, yylineno}); }
struct_body:
	/*Empty body allowed, because why not */ {$<nval>$ = new_list_node(0, (origin){filename, yylineno}); }
	| name_type_pair {$<nval>$ = new_list_node(10, (origin){filename, yylineno}); add_to_list($<nval>$, $<nval>1); }
	| struct_body LINE_END name_type_pair { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
struct_def:
	TYPE_TOKEN name STRUCT_TOKEN LINE_END struct_body LINE_END END_TOKEN { $<nval>$ = new_binary_node(STRUCT_DELARATION, $<nval>2, $<nval>5, (origin){filename, yylineno}); }
//FUNCTION DECLARATIONS
function_params:
	/*Empty body allowed, because why not */ {$<nval>$ = new_list_node(0, (origin){filename, yylineno}); }
	| name_type_pair {$<nval>$ = new_list_node(10, (origin){filename, yylineno}); add_to_list($<nval>$, $<nval>1); }
	| function_params ',' name_type_pair { add_to_list($<nval>1, $<nval>3); $<nval>$ = $<nval>1; }
block:
	/*Empty block*/ { $<nval>$ = new_list_node(10, (origin){filename, yylineno}); }
	| block statement { add_to_list($<nval>1, $<nval>2); $<nval>$ = $<nval>1; }
function_def:
	FUNC_TOKEN name '(' function_params ')' type LINE_END block END_TOKEN {
		node *func = malloc(sizeof(node));
		func->type = FUNCTION_DECLARATION;
		func->data.func.name = $<nval>2;
		func->data.func.return_type = $<nval>6;
		func->data.func.params = $<nval>4;
		func->data.func.body = $<nval>8;
		func->origin = (origin){filename, yylineno};
		$<nval>$ = func; }
statement:
	expression LINE_END
	| control LINE_END
	| variable_declaration LINE_END
	| RETURN_TOKEN expression LINE_END { $<nval>$ = new_unary_node(RETURN, $<nval>2, (origin){filename, yylineno}); }
//CONTROL STRUCTURES
else_list:
	ELSE_IF_TOKEN expression LINE_END block { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, NULL, (origin){filename, yylineno}); }
	| else_list else_alone { $<nval>$->data.ternary[2] = $<nval>2; }
	| else_list ELSE_IF_TOKEN expression LINE_END block { $<nval>$->data.ternary[2] = new_ternary_node(IF, $<nval>3, $<nval>5, NULL, (origin){filename, yylineno}); }
else_alone:
	ELSE_TOKEN LINE_END block { $<nval>$ = $<nval>3; }
control:
	IF_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, NULL, (origin){filename, yylineno}); }
	| WHILE_TOKEN expression LINE_END block END_TOKEN  { $<nval>$ = new_binary_node(WHILE, $<nval>2, $<nval>4, (origin){filename, yylineno});	}
	| IF_TOKEN expression LINE_END block else_alone END_TOKEN { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, $<nval>5, (origin){filename, yylineno}); }
	| IF_TOKEN expression LINE_END block else_list END_TOKEN { $<nval>$ = new_ternary_node(IF, $<nval>2, $<nval>4, $<nval>5, (origin){filename, yylineno}); }
//TYPES
type:
	WORD {
		node *expr = malloc(sizeof(node));
		expr->type = TYPE;
		expr->data.string = $1;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr;
	}
	| '@' type { $<nval>$ = new_unary_node(POINTER_OF, $<nval>2, (origin){filename, yylineno}); }
	| '[' ']' type  { $<nval>$ = new_unary_node(ARRAY_OF, $<nval>2, (origin){filename, yylineno});	}

//SINGLE TOKENS
name:
	WORD {
		node *expr = malloc(sizeof(node));
		expr->type = NAME;
		expr->data.string = $1;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr; }
number:
	INT {
		node *expr = malloc(sizeof(node));
		expr->type = NUM;
		expr->data.integer = $1;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr; }
string:
	STRING_TOKEN {
		node *expr = malloc(sizeof(node));
		expr->type = STRING;
		expr->data.string = $1;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr;
	}
boolean:
	TRUE_TOKEN {
		node *expr = malloc(sizeof(node));
		expr->type = VALUE_TRUE;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr; }
	| FALSE_TOKEN {
		node *expr = malloc(sizeof(node));
		expr->type = VALUE_FALSE;
		expr->origin = (origin){filename, yylineno};
		$<nval>$ = expr; }
%%
