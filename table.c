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

table *new_table(const table *parent) {
	table *tbl = new_root_table();
	tbl->parent = parent;
	return tbl;
}

void table_insert(table *tbl, const char *name, const node *declaration) {
	table_entry entry = {name, list_new(10, sizeof(node*))};
	list_add(&(entry.entries), declaration);
	list_add(&(tbl->entries), &entry);
}

table_entry *table_get(const table *tbl, const char *name) {
	for(unsigned int i = 0; i < tbl->entries.length; i++) {
		table_entry *entry = list_get(tbl->entries, i);
		if(strcmp(entry->name, name) == 0) {
			return (table_entry*)(tbl->entries.buffer) + i;
		}
	}
	if(tbl->parent != NULL) {
		return table_get(tbl->parent, name);
	} else {
		return NULL;
	}
}


void table_add(table *tbl, const char *name, const node *declaration) {
	table_entry *entry = table_get(tbl, name);
	if(entry == NULL) {
		table_insert(tbl, name, declaration);
	} else {
		list_add(&(entry->entries), declaration);
	}
}


void table_destroy(table *tbl) {
	list_destroy(tbl->entries);
	free(tbl);
}
