#include <Print.h>

enum type {
    UNASSIGNED,
    CONS,
    ARRAY,
    SYMBOL,
    STRING,
    CHARACTER,
    INTEGER,
    FLOAT,
    CFUNCTION,
    STREAM,
    ERROR
}
#define consp(x)           ((x) != nil && (x)->type == CONS)
#define arrayp(x)          ((x) != nil && (x)->type == ARRAY)
#define listp(x)           consp(x)
#define improperp(x)       (!listp(x))
#define atom(x)            (!consp(x))
#define integerp(x)        ((x) != nil && (x)->type == INTEGER)
#define floatp(x)          ((x) != nil && (x)->type == FLOAT)
#define numberp(x)         (integerp(x) || floatp(x))
#define symbolp(x)         ((x) != nil && (x)->type == SYMBOL)
#define keywordp(x)        (symbolp(x) && nthchar(x, 0) == ':')
#define stringp(x)         ((x) != nil && (x)->type == STRING)
#define characterp(x)      ((x) != nil && (x)->type == CHARACTER)
#define streamp(x)         ((x) != nil && (x)->type == STREAM)
#define errorp(x)          ((x) != nil && (x)->type == ERROR)

enum objectflags {
    GCMARK,
    PRMARK,
    PACKED,
    CAUGHT,
    BUILTIN,
}
#define oflag_set(x,f)     ((x)->objflags |= 1 << f)
#define oflag_clr(x,f)     ((x)->objflags &= ~(1 << f))
#define oflag_tst(x,f)     ((x)->objflags & 1 << f)

typedef struct sobject {
    uint8_t objflags;
    uint8_t type;
    union {
        struct { // type = CONS || type == ARRAY
            sobject *car;
            sobject *cdr;
        };
        struct { // type == STRING || type == SYMBOL || type == CHARACTER
            uint32_t chars;
            sobject *next;
        };
        int64_t intnum; // type == INTEGER
        double floatnum; // type == FLOAT
        struct { // type == CFUNCTION
            uint32_t info;
            fn_ptr_type *cfun;
        }
        struct { // type == STREAM
            sobject *name;
            Print *s;
        };
        struct { // type == ERROR
            uint32_t code;
            sobject *detail;
        };
    };
} object;

typedef object *(*fn_ptr_type)(object *, object *);

// New char
object *newchar(char c) {
    object *o = newobj();
    PR_ERR(o);
    o->type = CHARACTER;
    o->chars = c;
    o->next = nil;
    return o;
}
