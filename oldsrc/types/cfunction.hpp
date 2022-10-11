object *function(fn_ptr_type f) {
    object *o = newobj();
    PR_ERR(o);
    o->type = CFUNCTION;
    o->cfun = f;
    return o;
}

enum infoflag {
    SPECIAL = 0b000100000000,
    TAIL    = 0b001000000000,
}
