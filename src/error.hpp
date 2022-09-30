#define PROPAGATE_ERR(x) if (errorp(x) && !oflag_tst(x, CAUGHT)) return x

object *newerror(errorcode code, object *detail) {
    object *o = newobj();
    PROPAGATE_ERR(o);
    o->type = ERROR;
    o->code = code;
    o->detail = detail;
    return o;
}

#define THROW_ERR(c) return newerror(c, nil)
#define THROW_ERR(c, d) return newerror(c, d)