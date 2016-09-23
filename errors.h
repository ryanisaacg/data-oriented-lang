#pragma once

#include "origin.h"

typedef enum { ERROR_MISMATCHED_TYPES, ERROR_UNDELARED_VAR } error_type;

typedef struct {
	error_type type;
	origin origin;
	char *data;
} error;

void throw_error(error err);
