#include "expression.h"

#include <stdio.h>
#include <string.h>

void execute(node *root) {
	switch(root->type) {
	case ROOT:
		execute(root->data.root.body);
		break;
	case FUNC_CALL: {
		char *name = root->data.call.function->data.string;
		int param_count = root->data.call.num_params;
		if(strcmp(name, "print") == 0) {
			for(int i = 0; i < param_count; i++) {
				node *param = root->data.call.params[i];
				switch(param->type) {
				case STRING:
					printf("%s", param->data.string);
					break;
				case NUM:
					printf("%d", param->data.integer);
					break;
				default:
					fprintf(stderr, "Not currently implemented");
					break;
				}
			}
			printf("\n");
		}
		} break;
	default:
		fprintf(stderr, "Not currently implemented");
		break;
	}
}
