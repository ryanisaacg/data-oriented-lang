#include "table.h"

#include <stdlib.h>
#include <string.h>

#define TABLE_INIT_SIZE 10

table *new_root_table() {
	table *tbl = malloc(sizeof(table));
	tbl->parent = NULL;
	tbl->entries = list_new(10, sizeof(table_entry));
	return tbl;
}

table *new_table(table *parent) {
	table *tbl = new_root_table();
	tbl->parent = parent;
	return tbl;
}

void table_insert(table *tbl, char *name, node *declaration) {
	table_entry entry = {name, declaration};
	list_add(&(tbl->entries), &entry);
}

node *table_get(table *tbl, char *name) {
	for(unsigned int i = 0; i < tbl->entries.length; i++) {
		table_entry *entry = list_get(tbl->entries, i);
		if(strcmp(entry->name, name) == 0) {
			return entry->declaration;
		}
	}
	if(tbl->parent != NULL) {
		return table_get(tbl->parent, name);
	} else {
		return NULL;
	}
}

void table_destroy(table *tbl) {
	list_destroy(tbl->entries);
	free(tbl);
}
