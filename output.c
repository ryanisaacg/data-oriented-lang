#include "output.h"
#include <stdlib.h>

void c_write(FILE* stream, c_ast_node node) {
	fprintf(stream, "%s ", node.data);
	for(int i = 0; i < node.length; i++) {
		c_write(stream, node.list[i]);
	}
}

c_ast_node new_c_node(char *data, int children) {
	c_ast_node node = {data, malloc(sizeof(c_ast_node) * children), 0, children};
	return node;
}

void add_c_child(c_ast_node *parent, c_ast_node child) {
	if(parent->length < parent->capacity) {
		parent->list[parent->length] = child;
		parent->length++;
	} else {
		parent->list = realloc(parent->list, parent->capacity + 10);
		parent->capacity += 10;
		add_c_child(parent, child);
	}
}
