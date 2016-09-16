#include "analyzer.h"

#include "node.h"
#include "printing.h"
#include <stdio.h>
#include <stdlib.h>
#include "table.h"

static c_ast_node analyze_node(node *current, table *types, table *values);

c_ast_node analyze(rootnode root) {
	table *types = new_root_table();
	table *values = new_root_table();
	listnode struct_list = root.struct_list->data.list;
	listnode func_list = root.func_list->data.list;
	listnode main_list = root.main_list->data.list;
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
	analyze_node(m, types, values);
	add_c_child(&c_root, new_c_node("int main() {", 0));
	add_c_child(&c_root, analyze_node(m, types, values));
	add_c_child(&c_root, new_c_node("}", 0));
	return c_root;
}

static c_ast_node analyze_node(node *current, table *types, table *values) {
	switch(current->type) {
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
	default:
		printf("Unexpected node type in semantic analysis");
		return new_c_node("", 0);
		break;
	}
}
