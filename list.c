#include "list.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>

list list_new(size_t capacity, size_t size) {
	return (list){malloc(capacity * size), 0, capacity, size};
}

void list_add(list *l, const void *item) {
	if(l->length < l->capacity / 2) {
		memcpy(l->buffer + l->length * l->size, item, l->size);
		l->length++;
	} else {
		l->buffer = realloc(l->buffer, l->capacity * 2 * l->size);
		l->capacity *= 2;
		list_add(l, item);
	}
}

void *list_get(list l, size_t index) {
	if(index < l.length)
		return l.buffer + index * l.size;
	else {
		raise(SIGABRT);
		return NULL;
	}
}

void list_destroy(list l) {
	free(l.buffer);
}
