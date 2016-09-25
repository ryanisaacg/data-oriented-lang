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
	const type *modified;
} type_modifier;

typedef union {
	const struct node *declared;
	type_modifier modified;
	primitive number;
} type_data;

struct type {
	enum {DECLARATION, MODIFIER, NUMBER, C_BINDING, VOID} type;
	type_data data;
};

type *c_binding();
type *new_declared(const struct node *declared);
type *new_int(int bytes);
type *new_float(int bytes);
type *new_byte();
type *new_pointer(const type *wrapped);
type *new_array(const type *array);
const type *type_merge(const type *t1, const type *t2);
type *new_void();
bool equal(const type *t1, const type *t2);
