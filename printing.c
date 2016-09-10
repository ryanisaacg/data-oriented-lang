#include <stdio.h>
#include "printing.h"
#include "ast_strings.h"

static void print_expression_tabbed(node *expr, int tab) {
	printf("\n");
	for(int i = 0; i < tab; i++)
		printf("  ");
	printf("%s: ", statement_to_string(expr->type));
	switch(expr->type) {
	case OP_ADD:
	case OP_SUB:
		print_expression_tabbed(expr->data.binary.left, tab + 1);
		print_expression_tabbed(expr->data.binary.right, tab + 1);
		break;
	case FUNC_CALL:
		print_expression_tabbed(expr->data.call.function, tab + 1);
		for(int i = 0; i < expr->data.call.num_params; i++) {
			print_expression_tabbed(expr->data.call.params[i], tab + 1);
		}
		break;
	case NUM:
		printf("%d", expr->data.integer.value);
		break;
	case STRING:
		printf("%s", expr->data.string.str);
		break;
	case NAME:
		printf("%s", expr->data.string.str);
		break;
	default:
		fprintf(stderr, "Unexpected node type");
		break;
	}
}
		


void print_expression(node *expr) {
	print_expression_tabbed(expr, 0);
	printf("\n");
}
