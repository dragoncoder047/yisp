
// Yisp streams are this cons structure:
//
// [o|o]-->[o|o]-->pfun
//  |       |
//  v       v
//  name    gfun

#ifndef _y_stream_h_
#define _y_stream_h_
#include "../memory.h"

yobj *newstream(yisp_ctx *y, yobj *name, gfun_t g, pfun_t p);

#include "ystream.cpp"

#endif