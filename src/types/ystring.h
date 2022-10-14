
// Yisp strings

#ifndef _y_string_h_
#define _y_string_h_
#include "../memory.h"

yobj *ystring_frombuffer(yisp_ctx *y, char *buffer);
bool _y_buildstring(yisp_ctx *y, char ch, yobj **tail);
yobj *ystr_copy(yisp_ctx *y, yobj *str);
yobj *ystr_readstring(yisp_ctx *y, char delim, yobj *str);
size_t ystr_length(yobj *s);
char ystr_nthchar(yobj *string, size_t n);
bool ystr_isbuffer(yobj *s, char *buffer);
size_t ystr_tobuffer(yobj *s, char *buffer, size_t maxlen);

#include "ystring.cpp"

#endif