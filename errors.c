#include "errors.h"
#include <stdio.h>

static char* type_to_string(error_type type);

void throw_error(error err) {
	if(err.data != NULL)
		fprintf(stderr, "Error at line %d in file %s: %s\n(%s)\n", err.origin.line, err.origin.filename, type_to_string(err.type), err.data);
	else
		fprintf(stderr, "Error at line %d in file %s: %s\n", err.origin.line, err.origin.filename, type_to_string(err.type));
}

static char* type_to_string(error_type type) {
	switch(type) {
	case ERROR_MISMATCHED_TYPES:
		return "Mismatched types";
	case ERROR_UNDELARED_VAR:
		return "Unknown or undeclared variable";
	case ERROR_NO_SUCH_MEMBER:
		return "Not such member in type";
	}
	return "Unknown error type";
}
