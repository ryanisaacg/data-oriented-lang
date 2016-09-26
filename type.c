#include "type.h"
#include <math.h>
#include "node.h"
#include <stdlib.h>

type *c_binding() {
    type *t = malloc(sizeof(type));
    t->type = C_BINDING;
    return t;
}

type *new_declared(const node *declared) {
    type *t = malloc(sizeof(type));
    t->type = DECLARATION;
    t->data.declared = declared;
    return t;
}

type *new_int(int bytes) {
    type *t = malloc(sizeof(type));
    t->type = NUMBER;
    t->data.number.type = INT_TYPE;
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

type *new_pointer(const type *wrapped) {
    type *t = malloc(sizeof(type));
    t->type = MODIFIER;
    t->data.modified.type = POINTER;
    t->data.modified.modified = wrapped;
    return t;
}

type *new_array(const type *array) {
    type *t = malloc(sizeof(type));
    t->type = MODIFIER;
    t->data.modified.type = ARRAY;
    t->data.modified.modified = array;
    return t;
}

bool equal(const type *t1, const type *t2) {
    if(t1->type != t2->type) return false;
    switch(t1->type) {
    case MODIFIER:
        return t1->data.modified.type == t2->data.modified.type
            && equal(t1->data.modified.modified, t2->data.modified.modified);
    case DECLARATION:
        return t1->data.declared == t2->data.declared;
    case NUMBER:
        return t1->data.number.type == t2->data.number.type && t1->data.number.bytes == t2->data.number.bytes;
    case C_BINDING:
        return true;
    case VOID:
        return true;
    }
    return false;
}

const type *type_merge(const type *t1, const type *t2) {
	if(t1->type != t2->type && t2->type != C_BINDING) {
		if(t1->type == C_BINDING) {
			return type_merge(t2, t1);
		} else {
			return NULL;
		}
	} else {
		switch(t1->type) {
		case C_BINDING:
			return t1;
		case DECLARATION:
		case MODIFIER:
			if(equal(t1, t2))
				return t1;
			else
				return NULL;
		case NUMBER:
			if(equal(t1, t2))
				return t1;
			else {
				primitive pt1 = t1->data.number;
				primitive pt2 = t2->data.number;
				int bytes = pt1.bytes > pt2.bytes ? pt1.bytes : pt2.bytes;
				numeric_type num = pt1.type > pt2.type ? pt1.type : pt2.type;
				type *t = malloc(sizeof(type));
				t->type = NUMBER;
				t->data.number = (primitive){num, bytes};
				return t;
			}
        case VOID:
            return NULL;
		}
	}
	return NULL;
}

type *new_void() {
    type *t = malloc(sizeof(type));
    t->type = VOID;
    return t;
}
