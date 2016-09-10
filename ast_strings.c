#include "ast_strings.h"

char *statement_to_string(node_type type) {
	switch(type) {
        case OP_ADD:
        	return "OP_ADD";
        break;
        case OP_SUB:
        	return "OP_SUB";
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
		default:
			return "UNKNOWN_AST_TYPE";
		break;
	}
}
