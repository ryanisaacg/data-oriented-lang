#pragma once

#include "node.h"

typedef enum { TYPE_STRUCT } type_kind;

typedef struct {
	char *name;
	type_kind kind; 
	node *members;
} struct_declaration;


