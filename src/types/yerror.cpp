
#include "yerror.h"

// New error
yobj *yerror_new(yisp_ctx *y, yobj *message, yobj *where = NULL, yobj *detail = NULL) {
    yobj *e = y_newobj(y, ERROR);
    Y_RETURN_ON_NULL(e);
    e->message = message;
    yobj *d = NULL;
    if (!nullp(where) || !nullp(detail)) {
        yobj *d = y_newobj(y, ERRORDETAILS);
        if (!nullp(d)) {
            d->where = where;
            d->detail = detail;
        }
    }
    e->einfo = d;
    return e;
}
