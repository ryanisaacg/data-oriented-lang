#include "type.h"

#include <stdlib.h>

type *new_declared(node *declared) {
    type *t = malloc(sizeof(type));
    t->type = DECLARATION;
    t->data.declared = declared;
    return t;
}

type *new_int(int bytes) {
    type *t = malloc(sizeof(type));
    t->type = NUMBER;
    t->data.number.type = INT;
    t->data.number.bytes = bytes;
    return t;
}

type *new_float(int bytes) {
    type *t = malloc(sizeof(type));
    t->type = NUMBER;
    t->data.number.type = FLOAT;
    t->data.number.bytes = bytes;
    return t;
}

type *new_byte() {
    type *t = malloc(sizeof(type));
    t->type = NUMBER;
    t->data.number.type = BYTE;
    t->data.number.bytes = 8;
    return t;
}

type *new_pointer(type *wrapped) {
    type *t = malloc(sizeof(type));
    t->type = MODIFIER;
    t->data.modified.type = POINTER;
    t->data.modified.modified = wrapped;
    return t;
}

type *new_array(type *array) {
    type *t = malloc(sizeof(type));
    t->type = MODIFIER;
    t->data.modified.type = ARRAY;
    t->data.modified.modified = wrapped;
    return t;
}

bool equal(type *t1, type *t2) {
    if(t1->type != t2->type) return false;
    switch(t1->type) {
    case MODIFIER:
        return t1->data.modified.type == t2->data.modified.type
            && equal(t1->data.modified.modified, t2->data.modified.modified);
    case DECLARATION:
        return t1->data.declared == t2.data.declared;
    case NUMBER:
        return t1->data.number.type == t2->data.number.type && t1->data.number.bytes == t2->data.number.bytes;
    }
}
