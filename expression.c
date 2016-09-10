#include "expression.h"

#include <stdio.h>
#include <string.h>

void execute(node *root) {
	switch(root->type) {
	case ROOT:
		execute(root->data.root.body);
		break;
	case FUNC_CALL:
		char *name = root->call.function->string.str;
		int param_count = root->call.num_params;
		if(strcmp(name, "print") == 0) {
			for(int i = 0; i < param_count; i++) {
				node *param = root->call.params[i];
				switch(param->type) {
				case STRING:
					printf("%s", param->string.str);
					break;
				case NUM:
					printf("%d", param->integer.value);
					break;
				}
			}
			printf("\n");
		}
		break;
	}
}
