
#include "yfun.h"
#include "ycons.h"

yobj *yfun_new(yisp_env *y, uin8_t minargs, uint8_t maxargs, uint8_t flags, y_fun f) {
    yobj *o = y_newobj(y, CFUNCTION);
    Y_RETURN_ON_NULL(o);
    o->cfun = f;
    o->funinfo = (flags << 16) | ((minargs & 0x0F) << 4) | (maxargs & 0x0F);
    return o;
}

y_ckargs_result yfun_checkargs(yobj *f, yobj *args) {
    if (!cfunp(f)) return NOTAFUNCTION;
    size_t nargs = ycons_listlen(args);
    uint8_t minmax = f->funinfo & 0xFF;
    if (nargs < (minmax >> 4)) return TOOFEW;
    if ((minmax & 0x0F) != 0x0F && nargs > (minmax & 0x0F)) return TOOMANY;
    return OK;
}
