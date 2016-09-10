#include "expression.h"

#include <stdlib.h>

node *new_binary_node(node_type type, node *left, node *right) {
	node *new = malloc(sizeof(node));
	new->type = type;
	new->data.binary[0] = left;
	new->data.binary[1] = right;
	return new;
}
