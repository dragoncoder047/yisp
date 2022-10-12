
#include "ysymbol.h"
#include "ystring.h"

// New symbol
yobj *ysym_frombuffer(yisp_ctx *y, char *buffer) {
    yobj *str = ystring_frombuffer(y, buffer);
    Y_RETURN_IF_NULL(str);
    str->type = SYMBOL;
    return str;
}

// Looks up the symbol and creates it only if it is not found.
yobj *ysym_intern(yisp_ctx *y, char *buffer) {
    size_t ws = sizeof(y->workspace) / sizeof(yobj);
    for (size_t i = 0; i < ws; i++) {
        yobj *obj = &(y->workspace[i]);
        if (symbolp(obj) && ystr_isbuffer(obj, buffer)) return obj;
    }
    return ysym_frombuffer(y, buffer);
}
