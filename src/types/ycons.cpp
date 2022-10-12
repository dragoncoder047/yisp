
#include "ycons.h"

// New cons
yobj *y_cons(yisp_ctx *y, yobj *a, yobj *d) {
    Y_RETURN_ON_ERROR(a); Y_RETURN_ON_ERROR(d);
    yobj *o = y_newobj(y, CONS);
    Y_RETURN_ON_NULL(o);
    car(o) = a;
    cdr(o) = d;
    return o;
}

// get length of cons list
size_t ycons_listlen(yobj *list) {
    size_t length = 0;
    while (list != NULL) {
        if (improperp(list)) return -1;
        list = cdr(list);
        length++;
    }
    return length;
}

yobj *ycons_duplist(yisp_ctx *y, yobj *list) {
    if (nullp(list) || improperp(list)) return NULL;
    yobj *dup = y_cons(y, first(list), NULL);
    yobj *tail = dup;
    yobj *x;
    list = cdr(list);
    while (list != NULL) {
        x = y_cons(y, first(list), NULL);
        Y_RETURN_ON_NULL(x);
        cdr(tail) = x;
        tail = x;
        list = cdr(list);
    }
    return dup;
}