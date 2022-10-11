// New cons
object *cons(object *a, object *d) {
    PR_ERR(a, d);
    object *o = newobj();
    PR_ERR(o);
    o->type = CONS;
    car(o) = a;
    cdr(o) = d;
    return o;
}

// get length of cons list
uint32_t listlength(object *list) {
    uint32_t length = 0;
    while (list != NULL) {
        if (improperp(list)) THROW_VOIDERR(MALFORMEDLIST, list);
        list = cdr(list);
        length++;
    }
    return length;
}

#define car(x)             ((x)->car)
#define cdr(x)             ((x)->cdr)

#define first(x)           car(x)
#define second(x)          car(cdr(x))
#define cddr(x)            cdr(cdr(x))
#define third(x)           first(cddr(x))

#define push(x, y)         (y) = cons((x),(y)); PR_VOIDERR(y);
#define pop(y)             ((y) = cdr(y))