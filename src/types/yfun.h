
// C functions interface.

#ifndef _y_cfun_h_
#define _y_cfun_h_
#include "../memory.h"

enum infoflag {
    SPECIAL = 0b0001,
    TAIL    = 0b0010,
}

yobj *yfun_new(yisp_env *y, uin8_t minargs, uint8_t maxargs, uint8_t flags, y_fun f);

#include "yfun.cpp"

#endif