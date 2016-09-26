#include "node.h"

#include <stdlib.h>

node *new_type_node(type *semantic_type) {
	node *n = new_node(0, (origin){"", 0});
	n->semantic_type = semantic_type;
	return n;
}

node *new_node(node_type type, origin o) {
	node *new = malloc(sizeof(node));
	new->type = type;
	new->semantic_type = NULL;
	new->origin = o;
	return new;
}

node *new_ternary_node(node_type type, node *one, node *two, node *three, origin o) {
	node *new = new_node(type, o);
	new->data.ternary[0] = one;
	new->data.ternary[1] = two;
	new->data.ternary[2] = three;
	return new;
}

node *new_binary_node(node_type type, node *left, node *right, origin o) {
	node *new = new_node(type, o);
	new->data.binary[0] = left;
	new->data.binary[1] = right;
	return new;
}

node *new_unary_node(node_type type, node *operand, origin o) {
	node *expr = new_node(type, o);
	expr->data.unary = operand;
	return expr;
}

node *new_list_node(int initial_size, origin o) {
	node *new = new_node(LIST, o);
	new->data.list.data = malloc(sizeof(node) * initial_size);
	new->data.list.capacity = initial_size;
	new->data.list.length = 0;
	return new;
}

void add_to_list(node *list, node *item) {
	if(list->data.list.length < list->data.list.capacity) {
		list->data.list.data[list->data.list.length] = *item;
		list->data.list.length++;
	} else {
		list->data.list.data = realloc(list->data.list.data, list->data.list.capacity + 10);
		list->data.list.capacity += 10;
		add_to_list(list, item);
	}
}

node *get_from_list(node *list, int index) {
	return &(list->data.list.data[index]);
}
