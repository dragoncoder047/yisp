
// Comparing values.
#ifndef _y_cmp_h_
#define _y_cmp_h_
#include "memory.h"

bool y_eq(yobj *a, yobj *b);
bool y_equal(yobj *a, yobj *b);
bool y_cmpstr(yobj *arg1, yobj *arg2, bool lt, bool gt, bool eq);

#include "cmp.cpp"

#endif