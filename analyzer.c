#include "analyzer.h"

#include "node.h"
#include "printing.h"
#include <stdio.h>
#include <stdlib.h>
#include "table.h"

static c_ast_node analyze_node(node *current, table *types, table *values);
static c_ast_node binary_operator_left_typed(char *c_version, node *operator, table *types, table *values);
static c_ast_node binary_operator(char *c_version, node *operator, type *returned, table *types, table *values);
static c_ast_node analyze_block(node *list, table *types, table *values);

c_ast_node analyze(rootnode root) {
	table *types = new_root_table();
	table *values = new_root_table();
	listnode struct_list = root.struct_list->data.list;
	listnode func_list = root.func_list->data.list;
	listnode main_list = root.main_list->data.list;
	listnode external_list = root.ext_list->data.list;
	for(int i = 0; i < external_list.length; i++) {
		node *current = external_list.data[i];
		if(current->type == C_EXTERN) {
			table_insert(values, current->data.binary[0]->data.string, c_binding());
		}
	}
	for(int i = 0; i < struct_list.length; i++) {
		node *name = struct_list.data[i].data.binary[0];
		table_insert(types, name->data.string, struct_list.data + i);
	}
	for(int i = 0; i < func_list.length; i++) {
		node *name = func_list.data[i].data.func.name;
		table_insert(values, name->data.string, func_list.data + i);
	}
	c_ast_node c_root = new_c_node( "", func_list.length + 3);
	for(int i = 0; i < func_list.length; i++) {
		add_c_child(&c_root, analyze_node(func_list.data + i, types, values));
	}
	node *m = new_list_node(main_list.length);
	for(int i = 0; i < main_list.length; i++) {
		add_to_list(m, main_list.data + i);
	}
	analyze_block(m, types, values);
	add_c_child(&c_root, new_c_node("int main() {", 0));
	add_c_child(&c_root, analyze_block(m, types, values));
	add_c_child(&c_root, new_c_node("}", 0));
	return c_root;
}

static c_ast_node analyze_node(node *current, table *types, table *values) {
	switch(current->type) {
	case OP_ADD:
		return binary_operator_left_typed("+", current, types, values);
	case OP_SUB:
		return binary_operator_left_typed("-", current, types, values);
	case OP_MULT:
		return binary_operator_left_typed("*", current, types, values);
	case OP_EQUAL:
		return binary_operator("==", current, new_byte(), types, values);
	case OP_NOT_EQUAL:
		return binary_operator("!=", current, new_byte(), types, values);
	case OP_GREATER:
		return binary_operator(">", current, new_byte(), types, values);
	case OP_LESS:
		return binary_operator("<", current, new_byte(), types, values);
	case OP_LESS_EQUAL:
		return binary_operator("<=", current, new_byte(), types, values);
	case OP_GREATER_EQUAL:
		return binary_operator(">=", current, new_byte(), types, values);
	case NAME:
		current->semantic_type = new_declared(table_get(values, current->data.string));
		return new_c_node(current->data.string, 0);
	case NUM:
		//TODO: THIS IS DEFINITELY NOT A GOOD ASSUMTPION
		current->semantic_type = new_int(4);
		char *string = malloc(sizeof(char) * 10); //ALSO NOT A GOOD ASSUMPTION
		sprintf(string, "%d", current->data.integer);
		return new_c_node(string, 0);
	case TYPE:
		current->semantic_type = new_declared(table_get(values, current->data.string));
		return new_c_node(current->data.string, 0);
	case IF: {
		values = new_table(values);
		c_ast_node header = analyze_node(current->data.ternary[0], types, values);
		//TODO CHECK TYPES
		c_ast_node body = analyze_block(current->data.ternary[1], types, values);
		c_ast_node control;
		if(current->data.ternary[2] == NULL) {
			control = new_c_node("if(", 4);
			add_c_child(&control, header);
			add_c_child(&control, new_c_node("){", 0));
			add_c_child(&control, body);
			add_c_child(&control, new_c_node("}", 0));
		} else {
			c_ast_node elseclause;
			if(current->data.ternary[2]->type == LIST)
				elseclause = analyze_block(current->data.ternary[2], types, values);
			else
				elseclause = analyze_node(current->data.ternary[2], types, values);
			control = new_c_node("if(", 6);
			add_c_child(&control, header);
			add_c_child(&control, new_c_node("){", 0));
			add_c_child(&control, body);
			add_c_child(&control, new_c_node("} else {", 0));
			add_c_child(&control, elseclause);
			add_c_child(&control, new_c_node("}", 0));
		}
		return control;
	}
	case WHILE: {
		values = new_table(values);
		c_ast_node header = analyze_node(current->data.binary[0], types, values);
		//TODO CHECK TYPES
		c_ast_node body = analyze_block(current->data.binary[1], types, values);
		c_ast_node control = new_c_node("while(", 4);
		add_c_child(&control, header);
		add_c_child(&control, new_c_node("){", 0));
		add_c_child(&control, body);
		add_c_child(&control, new_c_node("}", 0));
		return control;
	}
	case FUNC_CALL: {
		char *name = current->data.binary[0]->data.string;
		current->semantic_type = new_declared(table_get(values, name));
		listnode list = current->data.binary[1]->data.list;
		c_ast_node call = new_c_node(name, 2 + list.length * 2 - 1);
		add_c_child(&call, new_c_node("(", 0));
		for(int i = 0; i < list.length; i++) {
			add_c_child(&call, analyze_node(&(list.data[i]), types, values));
			if(i < list.length - 1)
				add_c_child(&call, new_c_node(",", 0));
		}
		add_c_child(&call, new_c_node(")", 0));
		return call;
	}
	case OP_ASSIGN: {
		c_ast_node name = analyze_node(current->data.binary[0], types, values);
		c_ast_node value = analyze_node(current->data.binary[1], types, values);
		c_ast_node returned = new_c_node("", 3);
		c_ast_node equals = new_c_node("=", 0);
		add_c_child(&returned, name);
		add_c_child(&returned, equals);
		add_c_child(&returned, value);
		return returned;
	}
	case OP_INIT: {
		c_ast_node type = analyze_node(current->data.binary[0], types, values);
		int len = current->data.binary[1]->data.list.length;
		c_ast_node declarations = new_c_node("", len  * 3);
		for(int i = 0; i < len; i++) {
			add_c_child(&declarations, type);
			node *declared = &(current->data.binary[1]->data.list.data[i]);
			if(declared->type == OP_ASSIGN)
				table_insert(values, declared->data.binary[0]->data.string, current->data.binary[0]);
			else
				table_insert(values, declared->data.string, current->data.binary[0]);
			add_c_child(&declarations, analyze_node(declared, types, values));
			add_c_child(&declarations, new_c_node(";", 0));
		}
		return declarations;
	}
	case LIST: {
		int len = current->data.list.length;
		c_ast_node items = new_c_node("", len);
		for(int i = 0; i < len; i++) {
			add_c_child(&items, analyze_node(&(current->data.list.data[i]), types, values));
		}
		return items;
	}
	case FUNCTION_DECLARATION: {
		c_ast_node func = new_c_node("", 7);
		add_c_child(&func, analyze_node(current->data.func.return_type, types, values));
		add_c_child(&func, analyze_node(current->data.func.name, types, values));
		int length = current->data.func.params->data.list.length;
		c_ast_node parameters = new_c_node("(", length * 3);
		values = new_table(values);
		for(int i = 0; i < length; i++) {
			node **pair = current->data.func.params->data.list.data[i].data.binary;
			node *left = pair[0];
			node *right = pair[1];
			table_insert(values, right->data.string, left);
			add_c_child(&parameters, analyze_node(left, types, values));
			add_c_child(&parameters, analyze_node(right, types, values));
			if(i < length - 1) {
				add_c_child(&parameters, new_c_node(",", 0));
			}
		}
		add_c_child(&func, parameters);
		add_c_child(&func, new_c_node("){", 0));
		add_c_child(&func, analyze_block(current->data.func.body, types, values));
		add_c_child(&func, new_c_node("}", 0));
		return func;
	}
	case RETURN: {
		c_ast_node returned = new_c_node("return", 1);
		add_c_child(&returned, analyze_node(current->data.unary, types, values));
		return returned;
	}
	default:
		printf("Unexpected node type in semantic analysis: %d", current->type);
		return new_c_node("", 0);
		break;
	}
}

static c_ast_node binary_operator_left_typed(char *c_version, node *operator, table *types, table *values) {
	c_ast_node node = binary_operator(c_version, operator, new_declared(NULL), types, values);
	type *left = operator->data.binary[0]->semantic_type;
	if(left->type == C_BINDING) {
		operator->semantic_type = operator->data.binary[1]->semantic_type;
	} else {
		operator->semantic_type = left;
	}
	return node;
}

static c_ast_node binary_operator(char *c_version, node *operator, type *returned, table *types, table *values) {
	c_ast_node op = new_c_node("", 3);
	add_c_child(&op, analyze_node(operator->data.binary[0], types, values));
	add_c_child(&op, new_c_node(c_version, 0));
	add_c_child(&op, analyze_node(operator->data.binary[1], types, values));
	operator->semantic_type = returned;
	return op;
}

static c_ast_node analyze_block(node *lists, table *types, table *values) {
	listnode list = lists->data.list;
	c_ast_node items = new_c_node("", list.length * 2);
	for(int i = 0; i < list.length; i++) {
		add_c_child(&items, analyze_node(&(list.data[i]), types, values));
		add_c_child(&items, new_c_node(";", 0));
	}
	return items;
}
