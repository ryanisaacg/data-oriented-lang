#include "node.h"

#include <stdlib.h>

node *new_node(node_type type) {
	node *new = malloc(sizeof(node));
	new->type = type;
	new->semantic_type = NULL;
}

node *new_binary_node(node_type type, node *left, node *right) {
	node *new = new_node(type);
	new->data.binary[0] = left;
	new->data.binary[1] = right;
	return new;
}

node *new_unary_node(node_type type, node *operand) {
	node *expr = new_node(type);
	expr->data.unary = operand;
	return expr;
}

node *new_list_node(int initial_size) {
	node *new = new_node(LIST);
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
