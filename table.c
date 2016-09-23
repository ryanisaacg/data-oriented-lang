#include "table.h"

#include <stdlib.h>
#include <string.h>

#define TABLE_INIT_SIZE 10

table *new_root_table() {
	table *tbl = malloc(sizeof(table));
	tbl->parent = NULL;
	tbl->entries = malloc(sizeof(table_entry) * TABLE_INIT_SIZE);
	tbl->length = 0;
	tbl->capacity = TABLE_INIT_SIZE;
	return tbl;
}

table *new_table(table *parent) {
	table *tbl = new_root_table();
	tbl->parent = parent;
	return tbl;
}

void table_insert(table *tbl, char *name, node *declaration) {
	if(tbl->length < tbl->capacity) {
		tbl->entries[tbl->length].name = name;
		tbl->entries[tbl->length].declaration = declaration;
		tbl->length++;
	} else {
		tbl->entries = realloc(tbl->entries, sizeof(table_entry) * (tbl->capacity + 10));
		tbl->capacity += 10;
		table_insert(tbl, name, declaration);
	}
}

node *table_get(table *tbl, char *name) {
	for(int i = 0; i < tbl->length; i++) {
		table_entry entry = tbl->entries[i];
		if(strcmp(entry.name, name) == 0) {
			return entry.declaration;
		}
	}
	if(tbl->parent != NULL) {
		return table_get(tbl->parent, name);
	} else {
		return NULL;
	}
}

void table_destroy(table *tbl) {
	free(tbl->entries);
	free(tbl);
}
