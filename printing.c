#include <stdio.h>
#include "printing.h"

static char *statement_to_string(node_type type) {
	switch(type) {
        case OP_ADD:
        	return "OP_ADD";
        break;
        case OP_SUB:
        	return "OP_SUB";
        break;
        case OP_NEGATIVE:
			return "OP_NEGATIVE";
		break;
        case OP_MULT:
        	return "OP_MULT";
        break;
        case OP_DIV:
        	return "OP_DIV";
        break;
        case OP_MOD:
        	return "OP_MOD";
        break;
        case OP_EXP:
            return "OP_EXP";
        break;
        case OP_BIT_AND:
        	return "OP_BIT_AND";
        break;
        case OP_BOOL_AND:
        	return "OP_BOOL_AND";
        break;
        case OP_BIT_OR:
        	return "OP_BIT_OR";
        break;
        case OP_BOOL_OR:
        	return "OP_BOOL_OR";
        break;
        case OP_BIT_XOR:
        	return "OP_BIT_XOR";
        break;
        case OP_BOOL_XOR:
        	return "OP_BOOL_XOR";
        break;
        case OP_BIT_NOT:
        	return "OP_BIT_NOT";
        break;
        case OP_BOOL_NOT:
        	return "OP_BOOL_NOT";
        break;
        case OP_SHIFT_LEFT:
            return "OP_SHIFT_LEFT";
        break;
        case OP_SHIFT_RIGHT:
            return "OP_SHIFT_RIGHT";
        break;
        case OP_BIT_SHIFT_RIGHT:
            return "OP_BIT_SHIFT_RIGHT";
        break;
        case OP_GREATER:
        	return "OP_GREATER";
        break;
        case OP_LESS:
        	return "OP_LESS";
        break;
        case OP_GREATER_EQUAL:
        	return "OP_GREATER_EQUAL";
        break;
        case OP_LESS_EQUAL:
        	return "OP_LESS_EQUAL";
        break;
        case OP_EQUAL:
        	return "OP_EQUAL";
        break;
        case OP_NOT_EQUAL:
        	return "OP_NOT_EQUAL";
        break;
        case OP_ASSIGN:
        	return "OP_ASSIGN";
        break;
		case OP_INIT:
			return "OP_INIT";
		break;
        case OP_MEMBER:
        	return "OP_MEMBER";
        break;
        case OP_GETREF:
        	return "OP_GETREF";
        break;
        case OP_DEREF:
        	return "OP_DEREF";
        break;
        case OP_INDEX:
        	return "OP_INDEX";
        break;
        case RETURN:
        	return "RETURN";
        break;
        case CONTINUE:
        	return "CONTINUE";
        break;
        case BREAK:
            return "BREAK";
        break;
        case FUNC_CALL:
        	return "FUNC_CALL";
        break;
        case STACK_INIT:
        	return "STACK_INIT";
        break;
        case HEAP_INIT:
        	return "HEAP_INIT";
        break;
        case IF:
        	return "IF";
        break;
		case ELSE:
			return "ELSE";
		break;
        case WHILE:
        	return "WHILE";
        break;
        case FOR:
        	return "FOR";
        break;
        case FOREACH:
        	return "FOREACH";
        break;
        case BLOCK:
        	return "BLOCK";
        break;
        case TYPE:
        	return "TYPE";
        break;
        case NAME:
        	return "NAME";
        break;
        case ROOT:
        	return "ROOT";
        break;
        case STRING:
        	return "STRING";
        break;
        case CHAR:
        	return "CHAR";
        break;
        case NUM:
        	return "NUM";
        break;
        case VALUE_TRUE:
            return "TRUE";
        break;
        case VALUE_FALSE:
            return "FALSE";
        break;
		case STRUCT_MEMBER:
			return "MEMBER";
		break;
		case STRUCT_DELARATION:
			return "STRUCT";
		break;
		case FUNCTION_DECLARATION:
			return "FUNC";
		break;
		case LIST:
			return "LIST";
		break;
		default:
			return "UNKNOWN_AST_TYPE";
		break;
	}
}

static void print_expression_tabbed(node *expr, int tab) {
	printf("\n");
	for(int i = 0; i < tab; i++)
		printf("  ");
	printf("%s: ", statement_to_string(expr->type));
	switch(expr->type) {
	case OP_NEGATIVE:
		print_expression_tabbed(expr->data.unary, tab + 1);
		break;
	case OP_ADD:
	case OP_SUB:
	case OP_MULT:
	case OP_DIV:
	case OP_EXP:
	case OP_MOD:
	case STRUCT_MEMBER:
	case STRUCT_DELARATION:
	case FUNC_CALL:
	case IF:
	case WHILE:
		print_expression_tabbed(expr->data.binary[0], tab + 1);
		print_expression_tabbed(expr->data.binary[1], tab + 1);
		break;
		break;
	case NUM:
		printf("%d", expr->data.integer);
		break;
	case STRING:
	case TYPE:
	case NAME:
		printf("%s", expr->data.string);
		break;
	case LIST:
		for(int i = 0; i < expr->data.list.length; i++) {
			print_expression_tabbed(get_from_list(expr, i), tab + 1);
		}
		break;
	case FUNCTION_DECLARATION:
		print_expression_tabbed(expr->data.func.name, tab + 1);
		print_expression_tabbed(expr->data.func.return_type, tab + 1);
		print_expression_tabbed(expr->data.func.params, tab + 1);
		print_expression_tabbed(expr->data.func.body, tab + 1);
	case ROOT:
		break;
	default:
		fprintf(stderr, "Unexpected node type: %d", expr->type);
		break;
	}
}
		


void print_expression(node *expr) {
	print_expression_tabbed(expr, 0);
	printf("\n");
}
