#pragma once

struct node; //Including node.h causes circular imports

#include <stdbool.h>

struct type;
typedef struct type type;

typedef enum {FLOAT, INT, BYTE} numeric_type;

typedef struct {
	numeric_type type;
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

type *new_declared(node *declared);
type *new_int(int bytes);
type *new_float(int bytes);
type *new_byte();
type *new_pointer(type *wrapped);
type *new_array(type *array);
bool equal(type *t1, *type t2);
