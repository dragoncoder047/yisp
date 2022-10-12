
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
