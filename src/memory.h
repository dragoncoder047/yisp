// Typedef for Yisp object & environment
// Inlined type predicates
// Garbage collection
// Make new object
#ifndef _y_memory_h_
#define _y_memory_h_
#include <cstdint>
#include <cstring>
#include <cstddef>
#include "memory.cpp"

enum y_type {
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
    STREAMINNER,
    ERROR,
    ERRORDETAILS,
    TOKEN,
}

enum y_object_flag {
    GCMARK,
    PRMARK,
    CAUGHT,
}

typedef struct syobj {
    uint8_t objflags;
    y_type type;
    union {
        // struct'ured like this so all "car" things and all "cdr" things are aligned with each other.
        struct {
            union {
                syobj *car; // type = CONS || type == ARRAY
                syobj *name; // type == STREAM
                syobj *message; // type == ERROR
                syobj *where; // type == ERRORDETAILS
                uint32_t chars; // type == STRING || type == SYMBOL || type == CHARACTER || type == TOKEN
                uint32_t funinfo; // type == CFUNCTION
                gfun_t gfun; // type == STREAMINNER
            };
            union {
                syobj *cdr; // type = CONS || type == ARRAY
                syobj *next; // type == STRING || type == SYMBOL
                syobj *funs; // type == STREAM
                syobj *einfo; // type == ERROR
                syobj *detail; // type == ERRORDETAILS
                y_fun *cfun; // type == CFUNCTION
                pfun_t pfun; // type == STREAMINNER
            };
        };
        // these take up all 64 bits
        int64_t intnum; // type == INTEGER
        double floatnum; // type == FLOAT
    };
} yobj;

// Object flags
inline void yoflag_set(yobj *x, y_object_flag f) { x->objflags |= 1 << f; }
inline void yoflag_clr(yobj *x, y_object_flag f) { x->objflags &= ~(1 << f); }
inline bool yoflag_tst(yobj *x, y_object_flag f) { return x->objflags & (1 << f); }

// Type predicates
inline bool nullp(yobj *x)      { return x == NULL; }
inline bool consp(yobj *x)      { return !nullp(x) && x->type == CONS; }
inline bool arrayp(yobj *x)     { return !nullp(x) && x->type == ARRAY; }
inline bool listp(yobj *x)      { return consp(x); }
inline bool improperp(yobj *x)  { return !listp(x); }
inline bool atom(yobj *x)       { return !listp(x); }
inline bool integerp(yobj *x)   { return !nullp(x) && x->type == INTEGER; }
inline bool floatp(yobj *x)     { return !nullp(x) && x->type == FLOAT; }
inline bool numberp(yobj *x)    { return integerp(x) || floatp(x); }
inline bool symbolp(yobj *x)    { return !nullp(x) && x->type == SYMBOL; }
inline bool stringp(yobj *x)    { return !nullp(x) && x->type == SYMBOL; }
inline bool ssp(yobj *x)        { return stringp(x) || symbolp(x); }
inline bool characterp(yobj *x) { return !nullp(x) && x->type == CHARACTER; }
inline bool streamp(yobj *x)    { return !nullp(x) && x->type == STREAM; }
inline bool errorp(yobj *x)     { return !nullp(x) && x->type == ERROR; }
inline bool cfunp(yobj *x)      { return !nullp(x) && x->type == CFUNCTION; }
// this one is not inlined because it is more complicated
bool keywordp(yobj *x);

// basic access function
#define car(x) ((x)->car)
#define cdr(x) ((x)->cdr)

#define first(x) car(x)
#define second(x) car(cdr(x))
#define cddr(x) cdr(cdr(x))
#define third(x) first(cddr(x))

// Common stack management function
#define push(y,i,s) (s) = y_cons((y), (i), (s)); Y_RETURN_ON_NULL(s);
#define pop(s) (s) = cdr(s);

// FFI function
typedef yobj *(*y_fun)(yisp_ctx *, yobj *, yobj *);

// Stream interface functions
typedef char (*gfun_t)();
typedef void (*pfun_t)(char);

template<int workspace_size>
typedef struct s_ctx {
    yobj workspace[workspace_size];
    size_t free_space;
    yobj* free_list;
    yobj* gc_stack;
    yobj* global_env;
    yobj* tee;
    uint16_t flags;
    // For reader/printer
    char last_char;
    char last_print;
} yisp_ctx;

// Error propagating macros
#define Y_IF_ERROR(x) if (errorp(x) && !yoflag_tst((x), CAUGHT));
#define Y_RETURN_ON_ERROR(x) Y_IF_ERROR(x) return (x);
#define Y_RETURN_ON_NULL(x) if (nullp(x)) return NULL;

// Alloc and dealloc

// makes a new object
yobj *y_newobj(yisp_ctx *y, y_type t);

// puts the object into the Freelist
inline void y_freeobj(yisp_ctx *y, yobj *obj) {
    memset(obj, 0, sizeof(yobj));
    car(obj) = NULL;
    cdr(obj) = y->free_list;
    y->free_list = obj;
    y->free_space++;
}

// Garbage collection

void y_init(yisp_ctx *y);
void ygc_markobject(yobj *obj);
void ygc_sweep(yisp_ctx *y);
size_t ygc_collect(yisp_ctx *y);

// Misc function

yobj *y_pun(yisp_ctx *y, yobj *obj);

#endif
