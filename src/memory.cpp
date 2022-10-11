#include "memory.h"
#include "types/ystring.h"

// To be a keyword, the first char has to be a colon and the second has to be a letter.
bool keywordp(yobj *x) {
    if (!symbolp(x)) return false;
    if (ystr_nthchar(x, 0) != ':') return false;
    char c1 = ystr_nthchar(x, 1);
    c1 |= 0x20;
    return 'A' <= c1 && c1 <= 'Z';
}

yobj *y_newobj(yisp_ctx *y, y_type t = UNASSIGNED)  {
    if (y->free_space == 0) return NULL;
    yobj *temp = y->free_list;
    y->free_list = y->free_list->cdr;
    y->free_space--;
    temp->type = t;
    return temp;
}

// Garbage collection

void y_init(yisp_ctx *y) {
    // Init freelist.
    y->free_list = NULL;
    y->free_space = 0;
    size_t ws = sizeof(y->workspace) / sizeof(yobj);
    for (size_t i = ws - 1; i>=0; i--) {
        yobj *obj = &(y->workspace[i]);
        car(obj) = NULL;
        cdr(obj) = y->free_list;
        y->free_list = obj;
        y->free_space++;
    }
    // Init env.
    y->gc_stack = NULL;
    y->global_env = NULL;
}

void ygc_markobject(yobj *obj) {
    MARK:
    if (nullp(obj)) return;
    if (yoflag_tst(obj, GCMARK)) return;
    yoflag_set(obj, GCMARK);
    switch (obj->type) {
        case SYMBOL:
        case STRING:
            while (obj != NULL) {
                yoflag_set(obj, GCMARK);
                obj = cdr(obj);
            }
            break;
        case CHARACTER:
        case INTEGER:
        case FLOAT:
        case CFUNCTION:
        case STREAMINNER:
        case UNASSIGNED:
        case TOKEN:
            break;
        case CONS:
        case ARRAY:
        case ERROR:
        case STREAM:
        case ERRORDETAILS:
            ygc_markobject(car(obj));
            obj = cdr(obj);
            goto MARK;
    }
}

void ygc_sweep(yisp_ctx *y) {
    y->free_list = NULL;
    y->free_space = 0;
    size_t ws = sizeof(y->workspace) / sizeof(yobj);
    for (size_t i = ws - 1; i >= 0; i--) {
        yobj *obj = &(y->workspace[i]);
        if (!yoflag_tst(obj, GCMARK)) y_freeobj(y, obj); else yoflag_clr(obj, GCMARK);
    }
}

size_t ygc_collect(yisp_ctx *y, yobj *form, yobj *env) {
    size_t start = y->free_space;
    ygc_markobject(y->global_env);
    ygc_markobject(y->gc_stack);
    ygc_markobject(y->tee);
    ygc_markobject(form);
    ygc_markobject(env);
    ygc_sweep(y);
    return y->free_space - start;
}

yobj *y_pun(yisp_ctx *y, yobj *obj, y_type newtype) {
    if (nullp(obj)) return NULL;
    yobj *n = y_newobj(y, obj->type);
    memcpy(n, obj, sizeof(yobj));
    n->type = newtype;
    return n;
}
