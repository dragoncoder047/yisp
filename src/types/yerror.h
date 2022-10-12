
// Yisp errors: not thrown, they are returned!

#ifndef _y_error_h_
#define _y_error_h_
#include "../memory.h"

yobj *yerror_new(yisp_ctx *y, yobj *message, yobj *where = NULL, yobj *detail = NULL);

#include "yerror.cpp"

#endif