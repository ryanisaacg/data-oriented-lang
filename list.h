#pragma once

#include <stddef.h>

typedef struct {
	char *buffer;
	size_t length, capacity, size;
} list;

list list_new(size_t capacity, size_t size);
void list_add(list *l, void *item);
void *list_get(list l, size_t index);
void list_destroy(list l);
