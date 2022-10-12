
#include "yfun.h"

yobj *yfun_new(yisp_env *y, uin8_t minargs, uint8_t maxargs, uint8_t flags, y_fun f) {
    yobj *o = y_newobj(y, CFUNCTION);
    Y_RETURN_ON_NULL(o);
    o->cfun = f;
    o->info = (flags << 16) | ((minargs & 0x0F) << 4) | (maxargs & 0x0F);
    return o;
}
