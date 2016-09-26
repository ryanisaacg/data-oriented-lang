#pragma once

#include "origin.h"

typedef enum { ERROR_MISMATCHED_TYPES, ERROR_UNDELARED_VAR, ERROR_NO_SUCH_MEMBER,
	ERROR_REDECLARATION, ERROR_TYPE_NOT_FOUND } error_type;

typedef struct {
	error_type type;
	origin origin;
	char *data;
} error;

void throw_error(error err);
