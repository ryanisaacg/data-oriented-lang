#include "output.h"
#include <stdlib.h>

void c_write(FILE* stream, c_ast_node node) {
	
}

c_ast_node new_c_node(c_ast_type type, char *data, int children) {
	c_ast_node node = {type, data, malloc(sizeof(c_ast_node) * children), 0, children};
}

void add_c_child(c_ast_node *parent, c_ast_node child) {
	if(parent->length < parent->capacity) {
		parent->children[parent->length] = child;
		parent->length++;
	} else {
		parent->children = realloc(parent->children, parent->capacity + 10);
		add_c_child(parent, child);
	}
}