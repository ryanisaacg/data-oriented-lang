#pragma once

#include <stdio.h>

typedef enum { BINARY_OPERATOR, UNARY_OPERATOR, WORD, CONTROL, FOR, CALL, LIST, TYPE, FUNCTION, STRUCT} c_ast_type;

struct c_ast_node;
typedef struct c_ast_node c_ast_node;

struct c_ast_node {
	c_ast_type type;
	char *data;
	c_ast_node *list;
	int children;
};

void c_write(FILE* stream, c_ast_node node);