#define nil NULL

#define car(x)             (((object *) (x))->car)
#define cdr(x)             (((object *) (x))->cdr)

#define first(x)           car(x)
#define second(x)          car(cdr(x))
#define cddr(x)            cdr(cdr(x))
#define third(x)           first(cddr(x))

#define push(x, y)         ((y) = cons((x),(y)))
#define pop(y)             ((y) = cdr(y))

// type predicates
#define consp(x)           ((x) != NULL && (x)->type == CONS)
#define listp(x)           consp(x)
#define improperp(x)       (!listp(x))
#define atom(x)            (!consp(x))
#define integerp(x)        ((x) != NULL && (x)->type == INTEGER)
#define floatp(x)          ((x) != NULL && (x)->type == FLOAT)
#define symbolp(x)         ((x) != NULL && (x)->type == SYMBOL)
#define stringp(x)         ((x) != NULL && (x)->type == STRING)
#define characterp(x)      ((x) != NULL && (x)->type == CHARACTER)
#define streamp(x)         ((x) != NULL && (x)->type == STREAM)
#define errorp(x)          ((x) != NULL && (x)->type == ERROR)

#define oflag_set(x,f)     ((x)->objflags |= 1 << f)
#define oflag_clr(x,f)     ((x)->objflags &= ~(1 << f))
#define oflag_tst(x,f)     ((x)->objflags & 1 << f)

#define mark(x)            oflag_set(x,GCMARK)
#define unmark(x)          oflag_clr(x,GCMARK)
#define marked(x)          oflag_tst(x,GCMARK)

#define pr_mark(x)         oflag_set(x,PRMARK)
#define pr_unmark(x)       oflag_clr(x,PRMARK)
#define pr_marked(x)       oflag_tst(x,PRMARK)

// for the reader
#define issp(x)            (x == ' ' || x == '\n' || x == '\r' || x == '\t')
#define isbr(x)            (x == ')' || x == '(' || x == '"' || x == '#')