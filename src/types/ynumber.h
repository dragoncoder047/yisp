
// Numbers: Int, and float. No complex in here.

#ifndef _y_number_h_
#define _y_number_h_
#include "../memory.h"

enum ynum_int_format {
    DECIMAL,
    HEXADECIMAL,
    OCTAL,
    BINARY
}

enum ynum_float_format {
    SMART,
    SCIENTIFIC,
    PREFIX,
    DIGITS
}

yobj *ynum_newint(yisp_ctx *y, int64_t n, ynum_int_format format = DECIMAL);
yobj *ynum_newfloat(yisp_ctx *y, double f, ynum_float_format format = SMART);

#include "ynumber.cpp"

#endif