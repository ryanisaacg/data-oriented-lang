#pragma once

#include "list.h"
#include "node.h"

struct table;
typedef struct table table;

struct table_entry;
typedef struct table_entry table_entry;

struct table_entry {
	const char *name;
	const node *declaration;
	int length;
};

struct table {
	const table *parent;
	list entries;
};

table *new_root_table();
table *new_table(const table *parent);
void table_insert(table *tbl, const char *name, const node *declaration);
void table_add(table *tbl, const char *name, const node *declaration);
const table_entry *table_get(const table *tbl, const char *name);
void table_destroy(table *tbl);
