#pragma once

#include <stdio.h>

struct c_ast_node;
typedef struct c_ast_node c_ast_node;

struct c_ast_node {
	char *data;
	c_ast_node *list;
	int length, capacity;
};

void c_write(FILE* stream, c_ast_node node);
c_ast_node new_c_node(char *data, int children);
void add_c_child(c_ast_node *parent, c_ast_node child);
