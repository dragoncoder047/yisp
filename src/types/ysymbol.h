
// Yisp symbols: Same as string, just with symbol type id.

#ifndef _y_symbol_h_
#define _y_symbol_h_
#include "../memory.h"

yobj *ysym_frombuffer(yisp_ctx *y, char *buffer);
yobj *ysym_intern(yisp_ctx *y, char *buffer);

#include "ysymbol.cpp"

#endif