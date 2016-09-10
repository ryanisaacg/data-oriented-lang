#pragma once

struct node;
typedef struct node node;

typedef enum node_type { OP_ADD, OP_SUB, OP_MULT, OP_DIV, OP_MOD, OP_EXP,
	OP_BIT_AND,	OP_BOOL_AND, OP_BIT_OR, OP_BOOL_OR, OP_BIT_XOR, OP_BOOL_XOR, OP_BIT_NOT, OP_BOOL_NOT,
	OP_GREATER, OP_LESS, OP_GREATER_EQUAL, OP_LESS_EQUAL, OP_EQUAL, OP_NOT_EQUAL,
	OP_SHIFT_LEFT, OP_SHIFT_RIGHT, OP_BIT_SHIFT_RIGHT,
	OP_ASSIGN, OP_INIT, OP_MEMBER, OP_GETREF, OP_DEREF, OP_INDEX, OP_NAMESPACE,
	RETURN, BREAK, CONTINUE,
	FUNC_CALL,
	STACK_INIT, HEAP_INIT,
	IF, ELSE, WHILE, FOR, FOREACH,
	BLOCK, TYPE, NAME,
	ROOT,
	STRING, CHAR, NUM,
	VALUE_TRUE, VALUE_FALSE } node_type;
typedef struct { node *left, *right; } binarynode;
typedef struct { node *head, *body; } controlnode;
typedef struct { node *function, **params; int num_params; } callnode;
typedef struct { node *body; } rootnode;
typedef struct { char *str; } stringnode;
typedef struct { int value; } intnode;

typedef union node_data {
	controlnode control;
	binarynode binary;
	callnode call;
	rootnode root;
	stringnode string;
	intnode integer;
} node_data;

struct node {
	node_data data;
	node_type type;
};

void execute(node *root);
