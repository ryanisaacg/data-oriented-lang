#pragma once

struct node;

#include <stdbool.h>

struct type;
typedef struct type type;

typedef enum {BYTE, INT_TYPE, FLOAT} numeric_type; //In ascending order of precedence

typedef struct {
	numeric_type type;
	int bytes;
} primitive;

typedef struct {
	enum {POINTER, ARRAY} type;
	type *modified;
} type_modifier;

typedef union {
	struct node *declared;
	type_modifier modified;
	primitive number;
} type_data;

struct type {
	enum {DECLARATION, MODIFIER, NUMBER, C_BINDING} type;
	type_data data;
};

type *c_binding();
type *new_declared(struct node *declared);
type *new_int(int bytes);
type *new_float(int bytes);
type *new_byte();
type *new_pointer(type *wrapped);
type *new_array(type *array);
type *type_merge(type *t1, type *t2);
bool equal(type *t1, type *t2);
