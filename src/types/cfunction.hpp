object *function(fn_ptr_type f) {
    object *o = newobj();
    PR_ERR(o);
    o->type = CFUNCTION;
    o->cfun = f;
    return o;
}
