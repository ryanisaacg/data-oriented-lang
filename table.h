#pragma once

#include "list.h"
#include "node.h"

struct table;
typedef struct table table;

struct table_entry;
typedef struct table_entry table_entry;

struct table_entry {
	char *name;
	node *declaration;
};

struct table {
	table *parent;
	list entries;
};

table *new_root_table();
table *new_table(table *parent);
void table_insert(table *tbl, char *name, node *declaration);
node *table_get(table *tbl, char *name);
void table_destroy(table *tbl);
