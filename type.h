#pragma once

struct type;
typedef struct type type;

typedef struct {
	enum { FLOAT, INT, BYTE} type;
	int bytes;
} primitive;

typedef struct {
	enum {POINTER, ARRAY} type;
	type *modified;
} type_modifier; 

typedef union {
	node *declared;
	type_modifier modified;
	primitive number;
} type_data;

struct type {
	enum {DECLARATION, MODIFIER, NUMBER} type;
	type_data data;
};
