
#include "ystream.h"

yobj *newstream(yisp_ctx *y, yobj *name, gfun_t g, pfun_t p) {
    yobj *s = y_newobj(y, STREAM);
    yobj *f = y_newobj(y, STREAMINNER);
    Y_RETURN_ON_NULL(s); Y_RETURN_ON_NULL(f);
    s->name = name;
    s->funs = f;
    f->pfun = p;
    f->gfun = g;
    return s;
}
