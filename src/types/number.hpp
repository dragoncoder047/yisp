// New integer
object *newint(int64_t n) {
    object *o = neowobj();
    PR_ERR(o);
    o->type = INTEGER;
    o->intnum = n;
    return o;
}

// New float
object *newfloat(double f) {
    object *o = newobj();
    PR_ERR(o);
    o->type = FLOAT;
    o->floatnum = f;
    return o;
}
