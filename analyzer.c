#include "analyzer.h"

#include "errors.h"
#include "node.h"
#include "printing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "printing.h"

static c_ast_node analyze_node(node *current);
static c_ast_node binary_operator(char *c_version, node *operator);
static c_ast_node analyze_block(node *list);
static void add_flag(char **cflags, int *cflag_capacity, char *new_flag);
static void type_pass(node *current, table *types, const table *primitives, table *values);
static c_ast_node analyze_externs(listnode external_list, char **cflags, int *cflag_capacity, table *values);
static c_ast_node mangle(listnode func_list);

c_ast_node analyze(rootnode root, char **cflags, int *cflag_capacity) {
	table *types = new_root_table();
	table *values = new_root_table();
	//Add numeric/builtin types
	table *primitives = new_root_table();
	table_insert(primitives, "int", new_type_node(new_int(4)));
	table_insert(primitives, "void", new_type_node(new_void()));
	listnode struct_list = root.struct_list->data.list;
	listnode func_list = root.func_list->data.list;
	listnode main_list = root.main_list->data.list;
	//Collect all of the statements in the main
	node *main_items = new_list_node(main_list.length, (origin){"", 0});
	for(int i = 0; i < main_list.length; i++) {
		add_to_list(main_items, main_list.data + i);
	}
	table_insert(values, "printf", new_type_node(c_binding())); //Insert printf into the symbol table
	//Run type checking
	for(int i = 0; i < struct_list.length; i++)
		type_pass(struct_list.data + i, types, primitives, values);
	for(int i = 0; i < func_list.length; i++) {
		node *name = func_list.data[i].data.func.name;
		table_insert(values, name->data.string, func_list.data + i);
	}
	for(int i = 0; i < func_list.length; i++)
		type_pass(func_list.data + i, types, primitives, values);
	type_pass(main_items, types, primitives, values);
	//Create C ouptut
	c_ast_node c_root = new_c_node( "extern void printf(); extern void *memcpy(); extern void *malloc();", func_list.length + 7);
	c_ast_node forward_decs = new_c_node("", struct_list.length);
	c_ast_node struct_declarations = new_c_node("", struct_list.length);
	add_c_child(&c_root, analyze_externs(root.ext_list->data.list, cflags, cflag_capacity, values));
	table_destroy(types);
	table_destroy(values);
	//Forward declaration and name mangling
	add_c_child(&c_root, mangle(func_list));
	for(int i = 0; i < struct_list.length; i++) {
		node *name = struct_list.data[i].data.binary[0];
		table_insert(types, name->data.string, struct_list.data + i);
		c_ast_node namenode = new_c_node(name->data.string, 0);
		c_ast_node semicolon = new_c_node(";", 0);
		c_ast_node dec = new_c_node("struct", 6);
		add_c_child(&dec, namenode);
		add_c_child(&dec, semicolon);
		add_c_child(&dec, new_c_node("typedef struct", 0));
		add_c_child(&dec, namenode);
		add_c_child(&dec, namenode);
		add_c_child(&dec, semicolon);
		add_c_child(&forward_decs, dec);
		add_c_child(&struct_declarations, analyze_node(struct_list.data + i));
	}
	add_c_child(&c_root, forward_decs);
	add_c_child(&c_root, struct_declarations);
	for(int i = 0; i < func_list.length; i++) {
		add_c_child(&c_root, analyze_node(func_list.data + i));
	}
	add_c_child(&c_root, new_c_node("int main() {", 0));
	add_c_child(&c_root, analyze_block(main_items));
	add_c_child(&c_root, new_c_node("}", 0));
	return c_root;
}

static c_ast_node analyze_node(node *current) {
	switch(current->type) {
	case OP_ADD:
		return binary_operator("+", current);
	case OP_SUB:
		return binary_operator("-", current);
	case OP_MULT:
		return binary_operator("*", current);
	case OP_EQUAL:
		return binary_operator("==", current);
	case OP_NOT_EQUAL:
		return binary_operator("!=", current);
	case OP_GREATER:
		return binary_operator(">", current);
	case OP_LESS:
		return binary_operator("<", current);
	case OP_LESS_EQUAL:
		return binary_operator("<=", current);
	case OP_GREATER_EQUAL:
		return binary_operator(">=", current);
	case OP_ASSIGN:
		return binary_operator("=", current);
	case OP_MEMBER: {
		c_ast_node left = analyze_node(current->data.binary[0]);
		c_ast_node right = analyze_node(current->data.binary[1]);
		char *c_operator;
		const type *left_type = current->data.binary[0]->semantic_type;
		if(left_type->type == MODIFIER) {
			c_operator = "->";
		} else {
			c_operator = ".";
		}
		c_ast_node operator = new_c_node("", 3);
		add_c_child(&operator, left);
		add_c_child(&operator, new_c_node(c_operator, 0));
		add_c_child(&operator, right);
		return operator;
	}
	case OP_DEREF: {
		c_ast_node deref = new_c_node("*", 1);
		add_c_child(&deref, analyze_node( current->data.unary));
		return deref;
	}
	case OP_GETREF: {
		c_ast_node getref = new_c_node("&", 1);
		add_c_child(&getref, analyze_node(current->data.unary));
		return getref;
	}
	case HEAP_INIT: {
		node *operand = current->data.unary;
		c_ast_node type, initializer;
		if(operand->type == NUM) {
			type = new_c_node("int", 0); //TODO: NOT ALL NUMBERS ARE INTS
			initializer = new_c_node("(int){", 2);
			add_c_child(&initializer, analyze_node(operand));
			add_c_child(&initializer, new_c_node("}", 0));
		} else if(operand->type == TYPE_LITERAL) {
			type = analyze_node(operand->data.binary[0]);
			initializer = analyze_node(operand);
		}
		c_ast_node size = new_c_node("sizeof(", 2);
		add_c_child(&size, type);
		add_c_child(&size, new_c_node(")", 0));
		c_ast_node output = new_c_node("memcpy(malloc(", 6);
		add_c_child(&output, size);
		add_c_child(&output, new_c_node("), &", 0));
		add_c_child(&output, initializer);
		add_c_child(&output, new_c_node(", ", 0));
		add_c_child(&output, size);
		add_c_child(&output, new_c_node(");", 0));
		return output;
	}
	case STRING:
		return new_c_node(current->data.string, 0);
	case NAME:
		return new_c_node(current->data.string, 0);
	case NUM:
		//TODO: THIS IS DEFINITELY NOT A GOOD ASSUMTPION
		current->semantic_type = new_int(4);
		char *string = malloc(sizeof(char) * 10); //ALSO NOT A GOOD ASSUMPTION
		sprintf(string, "%d", current->data.integer);
		return new_c_node(string, 0);
	case TYPE:
		return new_c_node(current->data.string, 0);
	case IF: {
		c_ast_node header = analyze_node(current->data.ternary[0]);
		c_ast_node body = analyze_block(current->data.ternary[1]);
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
				elseclause = analyze_block(current->data.ternary[2]);
			else
				elseclause = analyze_node(current->data.ternary[2]);
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
		c_ast_node header = analyze_node(current->data.binary[0]);
		c_ast_node body = analyze_block(current->data.binary[1]);
		c_ast_node control = new_c_node("while(", 4);
		add_c_child(&control, header);
		add_c_child(&control, new_c_node("){", 0));
		add_c_child(&control, body);
		add_c_child(&control, new_c_node("}", 0));
		return control;
	}
	case FUNC_CALL: {
		const type *t = current->semantic_type;
		char *name = t->data.declared->data.binary[0]->data.string; //current->data.binary[0]->data.string;
		listnode list = current->data.binary[1]->data.list;
		c_ast_node call = new_c_node(name, 2 + list.length * 2 - 1);
		add_c_child(&call, new_c_node("(", 0));
		for(int i = 0; i < list.length; i++) {
			add_c_child(&call, analyze_node(&(list.data[i])));
			if(i < list.length - 1)
				add_c_child(&call, new_c_node(",", 0));
		}
		add_c_child(&call, new_c_node(")", 0));
		return call;
	}
	case OP_INIT: {
		c_ast_node type = analyze_node(current->data.binary[0]);
		int len = current->data.binary[1]->data.list.length;
		c_ast_node declarations = new_c_node("", len  * 3);
		for(int i = 0; i < len; i++) {
			add_c_child(&declarations, type);
			add_c_child(&declarations, analyze_node(&(current->data.binary[1]->data.list.data[i])));
			add_c_child(&declarations, new_c_node(";", 0));
		}
		return declarations;
	}
	case LIST: {
		int len = current->data.list.length;
		c_ast_node items = new_c_node("", len);
		for(int i = 0; i < len; i++) {
			add_c_child(&items, analyze_node(&(current->data.list.data[i])));
		}
		return items;
	}
	case STRUCT_DELARATION: {
		c_ast_node name = analyze_node( current->data.binary[0]);
		listnode list = current->data.binary[1]->data.list;
		c_ast_node declaration = new_c_node("struct", list.length + 3);
		add_c_child(&declaration, name);
		add_c_child(&declaration, new_c_node("{", 0));
		for(int i = 0; i < list.length; i++) {
			node pair_node = list.data[i];
			c_ast_node item = new_c_node("", 3);
			add_c_child(&item, analyze_node(pair_node.data.binary[0]));
			add_c_child(&item, new_c_node(pair_node.data.binary[1]->data.string, 1));
			add_c_child(&item, new_c_node(";", 0));
			add_c_child(&declaration, item);
		}
		add_c_child(&declaration, new_c_node("};", 0));
		return declaration;
	}
	case FUNCTION_DECLARATION: {
		c_ast_node func = new_c_node("", 7);
		add_c_child(&func, analyze_node(current->data.func.return_type));
		add_c_child(&func, analyze_node(current->data.func.name));
		int length = current->data.func.params->data.list.length;
		c_ast_node parameters = new_c_node("(", length * 3);
		for(int i = 0; i < length; i++) {
			node **pair = current->data.func.params->data.list.data[i].data.binary;
			node *left = pair[0];
			node *right = pair[1];
			add_c_child(&parameters, analyze_node(left));
			add_c_child(&parameters, analyze_node(right));
			if(i < length - 1) {
				add_c_child(&parameters, new_c_node(",", 0));
			}
		}
		add_c_child(&func, parameters);
		add_c_child(&func, new_c_node("){", 0));
		add_c_child(&func, analyze_block(current->data.func.body));
		add_c_child(&func, new_c_node("}", 0));
		return func;
	}
	case RETURN: {
		c_ast_node returned = new_c_node("return", 1);
		add_c_child(&returned, analyze_node(current->data.unary));
		return returned;
	}
	case POINTER_OF: {
		c_ast_node dec = new_c_node("", 2);
		add_c_child(&dec, analyze_node(current->data.unary));
		add_c_child(&dec, new_c_node("*", 0));
		return dec;
	}
	case TYPE_LITERAL: {
		c_ast_node literal = new_c_node("((", 4);
		add_c_child(&literal, analyze_node(current->data.binary[0]));
		add_c_child(&literal, new_c_node("){", 0));
		listnode list = current->data.binary[1]->data.list;
		c_ast_node items = new_c_node("", list.length * 2);
		c_ast_node comma = new_c_node(",", 0);
		for(int i = 0; i < list.length; i++) {
			add_c_child(&items, analyze_node(list.data + i));
			add_c_child(&items, comma);
		}
		add_c_child(&literal, items);
		add_c_child(&literal, new_c_node("})", 0));
		return literal;
	}
	default:
		printf("Unexpected node type in semantic analysis: %s\n", statement_to_string(current->type));
		return new_c_node("", 0);
		break;
	}
}

static void add_flag(char **cflags, int *cflag_capacity, char *new_flag) {
	int flag_length = strlen(*cflags);
	int newflag_length = strlen(new_flag);
	int new_length = flag_length + newflag_length;
	if(new_length < *cflag_capacity) {
		strcat(*cflags, new_flag);
	} else {
		char *old_flags = *cflags;
		*cflags = malloc(new_length * 2);
		*cflags[0] = '\0';
		strcat(*cflags, old_flags);
		*cflag_capacity = new_length * 2;
		add_flag(cflags, cflag_capacity, new_flag);
	}
}

static c_ast_node binary_operator(char *c_version, node *operator) {
	c_ast_node op = new_c_node("", 3);
	add_c_child(&op, analyze_node(operator->data.binary[0]));
	add_c_child(&op, new_c_node(c_version, 0));
	add_c_child(&op, analyze_node(operator->data.binary[1]));
	return op;
}

static c_ast_node analyze_block(node *lists) {
	listnode list = lists->data.list;
	c_ast_node items = new_c_node("", list.length * 2);
	for(int i = 0; i < list.length; i++) {
		add_c_child(&items, analyze_node(&(list.data[i])));
		add_c_child(&items, new_c_node(";", 0));
	}
	return items;
}

static void type_pass(node *current, table *types, const table *primitives, table *values) {
	switch(current->type) {
	case OP_ADD:
	case OP_SUB:
	case OP_MULT: {
		//Arithmetic operation
		node *left = current->data.binary[0];
		node *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		type_pass(right, types, primitives, values);
		const type *returned = type_merge(left->semantic_type, right->semantic_type);
		if(returned == NULL) {
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "Invalid operands"});
		}
		current->semantic_type = returned;
	} break;
	//Integer division by default is bad
	case OP_DIV: {
		node *left = current->data.binary[0];
		node *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		type_pass(right, types, primitives, values);
		const type *returned = type_merge(left->semantic_type, right->semantic_type);
		if(returned == NULL) {
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "Invalid operands"});
		}
		current->semantic_type = new_float(returned->data.number.bytes);
	} break;
	//Comparison operators
	case OP_EQUAL:
	case OP_NOT_EQUAL:
	case OP_GREATER:
	case OP_LESS:
	case OP_LESS_EQUAL:
	case OP_GREATER_EQUAL: {
		node *left = current->data.binary[0];
		node *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		type_pass(right,types, primitives, values);
		const type *returned = type_merge(left->semantic_type, right->semantic_type);
		if(returned == NULL) {
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "Invalid operands"});
		}
		current->semantic_type = new_byte();
	} break;
	case OP_MEMBER: {
		node *left = current->data.binary[0];
		node *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		type_pass(right, types, primitives, values);
		const node *declared;
		const type *left_type = left->semantic_type;
		//Disambiguiate pointer / dot operator for C
		if(left_type->type == MODIFIER)
			declared = left_type->data.modified.modified->data.declared;
		else
			declared = left_type->data.declared;
		//Search for right-hand name in the declaration
		char *name = right->data.string;
		listnode list = declared->data.binary[1]->data.list;
		current->semantic_type = NULL;
		for(int i = 0; i < list.length; i++) {
			if(strcmp(name, list.data[i].data.binary[1]->data.string) == 0) {
				current->semantic_type = list.data[i].data.binary[0]->semantic_type;
				break;
			}
		}
		if(current->semantic_type == NULL) {
			char *error_string = " not found in type ";
			char *typename = declared->data.binary[0]->data.string;
			char *err = malloc(strlen(error_string) + strlen(typename) + strlen(name) + 1);
			err[0] = '\0';
			strcat(err, name);
			strcat(err, error_string);
			strcat(err, typename);
			throw_error((error){ERROR_NO_SUCH_MEMBER, current->origin, err});
		}
	} break;
	case OP_DEREF: {
		node *operand = current->data.unary;
		type_pass(operand, types, primitives, values);
		if(operand->semantic_type->type != MODIFIER) {
			throw_error((error){ ERROR_MISMATCHED_TYPES, current->origin, "Dereferencing a non-pointer"});
		}
		current->semantic_type = operand->semantic_type->data.modified.modified;
	} break;
	case HEAP_INIT:
	case OP_GETREF: {
		node *operand = current->data.unary;
		type_pass(operand, types, primitives, values);
		current->semantic_type = new_pointer(operand->semantic_type);
	} break;
	case STRING: {
		current->semantic_type = new_array(new_byte());
	} break;
	case NAME: {
		const table_entry *type_source = table_get(values, current->data.string);
		if(type_source == NULL)
			type_source = table_get(types, current->data.string);
		if(type_source != NULL)
			current->semantic_type = ((node*)list_get(type_source->entries, 0))->semantic_type;
		else
			throw_error((error){ ERROR_NAME_NOT_FOUND, current->origin, current->data.string});
	} break;
	case NUM: {
		//TODO: THIS IS DEFINITELY NOT A GOOD ASSUMTPION
		current->semantic_type = new_int(4);
	} break;
	case TYPE: {
		const table_entry *n =  table_get(types, current->data.string);
		//The type is not found in the symbol table
		if(n == NULL) {
			n = table_get(primitives, current->data.string);
			if(n == NULL)
				throw_error((error){ERROR_TYPE_NOT_FOUND, current->origin, current->data.string});
			else
				current->semantic_type = new_declared(((node*)list_get(n->entries, 0)));
		}
		else
			current->semantic_type = new_declared(((node*)list_get(n->entries, 0)));
	} break;
	case IF: {
		type_pass(current->data.ternary[0], types, primitives, values);
		if(!equal(current->data.ternary[0]->semantic_type, new_byte())) { //TODO: Bool type
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "if statements must be provided a boolean"});
		}
		//create a new symbol table for the duration of the block
		values = new_table(values);
		type_pass(current->data.ternary[1], types, primitives, values);
		table_destroy(values);
		if(current->data.ternary[2] != NULL) {
			//create a new symbol table for the duration of the block
			values = new_table(values);
			type_pass(current->data.ternary[2], types, primitives, values);
			table_destroy(values);
		}
	} break;
	case WHILE: {
		type_pass(current->data.binary[0], types, primitives, values);
		if(!equal(current->data.ternary[0]->semantic_type, new_byte())) { //TODO: Bool type
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "while statements must be provided a boolean"});
		}
		//create a new symbol table for the duration of the block
		values = new_table(values);
		type_pass(current->data.binary[1], types, primitives, values);
		table_destroy(values);
	} break;
	case FUNC_CALL: {
		char *name = current->data.binary[0]->data.string;
		listnode l = current->data.binary[1]->data.list;
		for(int i = 0; i < l.length; i++)
			type_pass(&(l.data[i]), types, primitives, values);
		const table_entry *entry = table_get(values, name);
		if(entry == NULL)
			throw_error((error){ ERROR_NAME_NOT_FOUND, current->origin, current->data.string});
		const list entries = entry->entries;
		const node *overload = NULL;
		for(unsigned int i = 0; i < entries.length && overload == NULL; i++) {
			const node *declared = list_get(entries, i);
			const listnode params = declared->data.func.params->data.list;
			if(params.length != l.length) continue;
			overload = declared;
			for(int j = 0; j < params.length; j++) {
				node actual = l.data[j];
				node formal = params.data[j];
				if(!equal(formal.semantic_type, type_merge(actual.semantic_type, formal.semantic_type))) {
					overload = NULL;
					break;
				}
			}
		}
		//TODO: Throw error if no overload found
		current->semantic_type = new_declared(overload);
	} break;
	case OP_ASSIGN: {
		node *left = current->data.binary[0], *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		type_pass(right, types, primitives, values);
		const type *combined = type_merge(left->semantic_type, right->semantic_type);
		if(!equal(left->semantic_type, combined)) {
			throw_error((error){ERROR_MISMATCHED_TYPES, current->origin, "Invalid assignment"});
		}
	} break;
	case OP_INIT: {
		node *left = current->data.binary[0], *right = current->data.binary[1];
		type_pass(left, types, primitives, values);
		int len = right->data.list.length;
		for(int i = 0; i < len; i++) {
			node *declared = &(right->data.list.data[i]);
			declared->semantic_type = left->semantic_type;
			char *name = declared->type == OP_ASSIGN ? declared->data.binary[0]->data.string : declared->data.string;
			if(table_get(values, name) != NULL)
				throw_error((error){ERROR_REDECLARATION, current->origin, name});
			table_insert(values, name, left);
			type_pass(declared, types, primitives, values);
		}
	} break;
	case LIST: {
		int len = current->data.list.length;
		for(int i = 0; i < len; i++)
			type_pass(&(current->data.list.data[i]), types, primitives, values);
	} break;
	case STRUCT_DELARATION: {
		table_insert(types, current->data.binary[0]->data.string, current);
		type_pass(current->data.binary[0], types, primitives, values);
		listnode list = current->data.binary[1]->data.list;
		for(int i = 0; i < list.length; i++) {
			node pair_node = list.data[i];
			type_pass(pair_node.data.binary[0], types, primitives, values);
		}
	} break;
	case FUNCTION_DECLARATION: {
		type_pass(current->data.func.return_type, types, primitives, values);
		type_pass(current->data.func.name, types, primitives, values);
		int length = current->data.func.params->data.list.length;
		values = new_table(values);
		for(int i = 0; i < length; i++) {
			node **pair = current->data.func.params->data.list.data[i].data.binary;
			node *left = pair[0];
			node *right = pair[1];
			table_insert(values, right->data.string, left);
			type_pass(left, types, primitives, values);
			type_pass(right, types, primitives, values);
		}
		type_pass(current->data.func.body, types, primitives, values);
	} break;
	case RETURN:
		type_pass(current->data.unary, types, primitives, values);
		break;
	case POINTER_OF: {
		type_pass(current->data.unary, types, primitives, values);
		current->semantic_type = new_pointer(current->data.unary->semantic_type);
	} break;
	case TYPE_LITERAL: {
		type_pass(current->data.binary[0], types, primitives, values);
		listnode list = current->data.binary[1]->data.list;
		for(int i = 0; i < list.length; i++)
			type_pass(list.data + i, types, primitives, values);
		current->semantic_type = current->data.binary[0]->semantic_type;
	} break;
	default:
		printf("Unexpected node type in semantic analysis: %s\n", statement_to_string(current->type));
		break;
	}
}

static c_ast_node analyze_externs(listnode external_list, char **cflags, int *cflag_capacity, table *values) {
	c_ast_node externs = new_c_node( "", external_list.length);
	for(int i = 0; i < external_list.length; i++) {
		node current = external_list.data[i];
		switch(current.type) {
		case C_EXTERN: {
			node *n = malloc(sizeof(node));
			char *name = current.data.binary[0]->data.string;
			n->semantic_type = c_binding();
			table_insert(values, name, n);
			c_ast_node ext = new_c_node("extern", 2);
			add_c_child(&ext, new_c_node(name, 0));
			add_c_child(&ext, new_c_node("();", 0));
			add_c_child(&externs, ext);
		} break;
		case C_IMPORT: {
			char *include = "\n#include \"";
			char *name = current.data.binary[0]->data.string;
			char *end_include = ".h\"\n";
			int length = strlen(include) + strlen(name) + strlen(end_include) + 1;
			char *result = malloc(length);
			result[0] = '\0';
			strcat(result, include);
			strcat(result, name);
			strcat(result, end_include);
			add_c_child(&externs, new_c_node(result, 0));
			if(current.data.binary[1] != NULL) {
				char *path = current.data.binary[1]->data.string;
				char *include = malloc(strlen(path) + 3);
				include[0] = '\0';
				strcat(include, " -I");
				strcat(include, path);
				add_flag(cflags, cflag_capacity, include);
			}
		} break;
		case C_LINK: {
			char *folder = current.data.binary[0]->data.string;
			char *link_folder = malloc(strlen(folder) + 3);
			link_folder[0] = '\0';
			strcat(link_folder, " -L");
			strcat(link_folder, folder);
			add_flag(cflags, cflag_capacity, link_folder);
			listnode list = current.data.binary[1]->data.list;
			for(int i = 0; i < list.length; i++) {
				char *path = list.data[i].data.string;
				char *link = malloc(strlen(path) + 3);
				link[0] = '\0';
				strcat(link, " -l");
				strcat(link, path);
				add_flag(cflags, cflag_capacity, link);
			}
		} break;
		default:
			fprintf(stderr, "Unexpected node type in externals: %s\n", statement_to_string(current.type));
			break;
		}
	}
	return externs;
}

static c_ast_node mangle(listnode func_list) {
	list names = list_new(10, sizeof(char*)), amount = list_new(10, sizeof(int));
	c_ast_node all_mangled = new_c_node("", func_list.length);
	for(int i = 0; i < func_list.length; i++) {
		funcnode func = func_list.data[i].data.func;
		c_ast_node function = new_c_node("", 3);
		char number[10];
		number[0] = '\0';
		int index = -1;
		char *name = func.name->data.string;
		for(unsigned int j = 0; j < names.length; j++) {
			char *current = list_get(names, j);
			if(strcmp(name, current) == 0)
				index = i;
		}
		if(index == -1) {
			list_add(&names, name);
			int n = 0;
			list_add(&amount, &n);
			sprintf(number, "%d", 0);
		} else {
			int *n = list_get(names, index);
			*n += 1;
			sprintf(number, "%d", *n);
		}
		char *mangled = malloc(strlen(name) + strlen(number) + 1);
		mangled[0] = '\0';
		strcat(mangled, name);
		strcat(mangled, number);
		func.name->data.string = mangled;
		add_c_child(&function, analyze_node(func.return_type));
		add_c_child(&function, new_c_node(mangled, 0));
		add_c_child(&function, new_c_node("();", 0));
		add_c_child(&all_mangled, function);
	}
	return all_mangled;
}
