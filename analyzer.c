#include "analyzer.h"

#include "node.h"
#include "table.h"

static c_ast_node analyze_node(node *current, symbol_table *types, symbol_table *values);

c_ast_node analyze(rootnode root) {
	symbol_table *types = new_root_table();
	symbol_table *values = new_root_table();
	listnode struct_list = root.struct_list->data.list;
	listnode func_list = root.func_list->data.list;
	listnode main_list = root.main_list->data.list;
	for(int i = 0; i < struct_list.length; i++) {
		node *name = struct_list.data[i].data.binary[0]
		table_insert(types, name->data.string, struct_list[i]);
	}
	for(int i = 0; i < func_list.length; i++) {
		node *name = func_list.data[i].data.func.name;
		table_insert(values, name->data.string, func_list[i]);
	}
	c_ast_node c_root = new_c_node(ROOT, "", func_list.length + 1);
	for(int i = 0; i < func_list.length; i++) {
		add_c_child(&root, analyze_node(func, types, values));
	node *main = new_list_node(main_list.length);
	for(int i = 0; i < main_list.length; i++) {
		add_to_list(main, main_list.data[i]);
	}
	add_child(&root, analyze_node(main, types, values));
	return root;
}

static c_ast_node analyze_node(node *current, symbol_table *types, symbol_table *values) {
	switch(current->type) {
	
	}
}