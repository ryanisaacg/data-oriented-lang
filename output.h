#pragma once

#include <stdio.h>

typedef enum { C_BINARY_OPERATOR, C_UNARY_OPERATOR, C_WORD, C_CONTROL, C_FOR, C_CALL, C_LIST, C_TYPE, C_FUNCTION, C_STRUCT, C_ROOT} c_ast_type;

struct c_ast_node;
typedef struct c_ast_node c_ast_node;

struct c_ast_node {
	c_ast_type type;
	char *data;
	c_ast_node *list;
	int length, capacity;
};

void c_write(FILE* stream, c_ast_node node);
c_ast_node new_c_node(c_ast_type type, char *data, int children);
void add_c_child(c_ast_node *parent, c_ast_node child);