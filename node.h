#pragma once

#include "origin.h"
#include "type.h"

struct node;
typedef struct node node;

typedef enum node_type { OP_ADD, OP_SUB, OP_NEGATIVE, OP_MULT, OP_DIV, OP_MOD, OP_EXP,
	OP_BIT_AND,	OP_BOOL_AND, OP_BIT_OR, OP_BOOL_OR, OP_BIT_XOR, OP_BOOL_XOR, OP_BIT_NOT, OP_BOOL_NOT,
	OP_GREATER, OP_LESS, OP_GREATER_EQUAL, OP_LESS_EQUAL, OP_EQUAL, OP_NOT_EQUAL,
	OP_SHIFT_LEFT, OP_SHIFT_RIGHT, OP_BIT_SHIFT_RIGHT,
	OP_ASSIGN, OP_INIT, OP_MEMBER, OP_GETREF, OP_DEREF, OP_INDEX, OP_NAMESPACE,
	RETURN, BREAK, CONTINUE,
	FUNC_CALL, TYPE_LITERAL, ARRAY_LITERAL,
	STACK_INIT, HEAP_INIT,
	IF, ELSE, WHILE, FOR, FOREACH,
	BLOCK, TYPE, NAME,
	ROOT,
	STRING, CHAR, NUM,
	VALUE_TRUE, VALUE_FALSE,
	STRUCT_MEMBER, STRUCT_DELARATION, LIST, FUNCTION_DECLARATION,
	EXPORT, IMPORT, C_EXTERN, C_IMPORT, C_LINK,
	ARRAY_OF, POINTER_OF} node_type;
typedef struct { node *data; int length, capacity; } listnode;
typedef struct { node *name, *return_type, *params, *body; } funcnode;
typedef struct { node *struct_list, *func_list, *main_list, *ext_list; } rootnode;

struct node {
	union {
		node *unary;
		node *binary[2];
		node *ternary[3];
		rootnode root;
		listnode list;
		funcnode func;
		char *string;
		int integer;
	} data;
	node_type type;
	type *semantic_type;
	origin origin;
};

node *new_node(node_type type, origin o);
node *new_ternary_node(node_type type, node *one, node *two, node *three, origin o);
node *new_binary_node(node_type type, node *left, node *right, origin o);
node *new_unary_node(node_type type, node *operand, origin o);
node *new_list_node(int initial_size, origin o);
void add_to_list(node *list, node *item);
node *get_from_list(node *list, int index);
