#include <Arduino.h>

typedef struct sobject {
    uint8_t objflags;
    uint8_t type;
    union {
        struct { // type = CONS
            sobject *car;
            sobject *cdr;
        };
        struct { // type == STRING || type == SYMBOL || type == CHARACTER
            uint32_t chars;
            sobject *next;
        };
        int64_t intnum; // type == INTEGER
        double floatnum; // type == FLOAT
        struct { // type == CFUNCTION
            uint32_t info;
            fn_ptr_type *cfun;
        }
        struct { // type == STREAM
            sobject *name;
            Stream *s;
        };
        struct { // type == ERROR
            uint32_t code;
            sobject *detail;
        };
    };
} object;

typedef object *(*fn_ptr_type)(object *, object *);

uint32_t Freespace = 0;
object *Freelist;

object *newobj() {
    object *temp = Freelist;
    Freelist = cdr(Freelist);
    Freespace--;
    obj->objflags = 0;
    if (Freespace == 0) {
        obj->type = ERROR;
        obj->code = OUTOFMEMORY;
    }
    return temp;
}

inline void freeobj(object *obj) {
    car(obj) = NULL;
    cdr(obj) = Freelist;
    Freelist = obj;
    Freespace++;
}