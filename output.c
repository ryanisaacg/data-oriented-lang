#include "output.h"
#include <stdlib.h>

void c_write(FILE* stream, c_ast_node *node) {
	fprintf(stream, "%s ", node->data);
	for(unsigned int i = 0; i < node->children.length; i++) {
		c_write(stream,list_get(node->children, i));
	}
}

c_ast_node new_c_node(char *data, int children) {
	c_ast_node node = {data, list_new(children, sizeof(c_ast_node))};
	return node;
}

void add_c_child(c_ast_node *parent, c_ast_node child) {
	list_add(&(parent->children), &child);
}
