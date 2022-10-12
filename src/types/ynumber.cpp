
#include "ynumber.h"

// New integer
yobj *ynum_newint(yisp_ctx *y, int64_t n, ynum_int_format format = DECIMAL) {
    yobj *o = y_newobj(y, INTEGER);
    Y_RETURN_ON_NULL(o);
    o->intnum = n;
    o->objflags |= format << FORMAT_1;
    return o;
}

// New float
yobj *ynum_newfloat(yisp_ctx *y, double f, ynum_float_format format = SMART) {
    yobj *o = y_newobj(y, FLOAT);
    Y_RETURN_ON_NULL(o);
    o->floatnum = f;
    o->objflags |= format << FORMAT_1;
    return o;
}
