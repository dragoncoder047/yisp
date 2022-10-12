
// Cons lists: the main structure of Yisp/Lisp.

#ifndef _y_cons_h_
#define _y_cons_h_
#include "../memory.h"

yobj *y_cons(yisp_ctx *y, yobj *a, yobj *d);
size_t ycons_listlen(yobj *list);

#include "ycons.cpp"

#endif