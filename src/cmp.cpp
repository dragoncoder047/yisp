
#include "cmp.h"

bool y_eq(yobj *arg1, yobj *arg2) {
    if (arg1 == arg2) return true;  // Same object
    if (nullp(arg1) || nullp(arg2)) return false;  // Not both values
    if (arg1->intnum != arg2->intnum) return false;  // Different values
    if (arg1->type != arg2->type) return false; // Different type
    if (symbolp(arg1) || numberp(arg1) || characterp(arg1)) return true;  // Same symbol, integer, float, or character
    return false;
}

bool y_equal(yobj *arg1, yobj *arg2) {
    if (y_eq(arg1, arg2)) return true; // Eq
    if (arg1->type != arg2->type) return false; // Different type
    if (stringp(arg1)) return y_cmpstr(arg1, arg2, false, false, true);
    if (consp(arg1) || arrayp(arg1)) {
        while (!nullp(arg1) && !nullp(arg2)) {
            if (!equal(car(arg1), car(arg2))) return false;
            arg1 = cdr(arg1);
            arg2 = cdr(arg2);
        }
        return nullp(arg1) && nullp(arg2);
    }
    return false;
}

bool y_cmpstr(yobj *arg1, yobj *arg2, bool lt, bool gt, bool eq) {
    while (!nullp(arg1) || !nullp(arg2)) {
        if (nullp(arg1)) return lt;
        if (nullp(arg2)) return gt;
        if (arg1->chars < arg2->chars) return lt;
        if (arg1->chars > arg2->chars) return gt;
        arg1 = arg1->next;
        arg2 = arg2->next;
    }
    return eq;
}
