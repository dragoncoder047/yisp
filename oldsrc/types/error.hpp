enum errorcode {
    BREAK, // not really an error
    OUTOFMEMORY,
    NOTANUMBER,
    NOTANINT,
    NOTASTRING,
    NOTALIST,
    NOTASYMBOL,
    NOTASTREAM,
    NOTAFUNCTION,
    NOTDEFINED,
    MALFORMEDLIST,
    MALFORMEDPARAM,
    TOOMANYARGS,
    TOOFEWARGS,
    OVERFLOW,
    DIVBYZERO,
    INDEXNEGATIVE,
    INVALIDARG,
    MALFORMEDCLAUSE,
    OUTOFRANGE,
    CANTTAKECAR,
    CANTTAKECDR,
    CUSTOM=65535
}

// this is used to "return" an error from functions that can't return a Yisp object.
object *VoidErr;

// error macros
// throw with code only
#define THROW_ERR(c) return newerror(c, nil)
// throw with code and detail object
#define THROW_ERR(c, d) return newerror(c, d)
// throw to the VoidError pointer (code only)
#define THROW_VOIDERR(c) VoidErr = newerror(c, nil); return 0
// throw to the VoidError pointer (code and detail)
#define THROW_VOIDERR(c, d) VoidErr = newerror(c, d); return 0
// propagate VoidError -> returned error
// put this after every function call that may cause an error but can't return one
#define PR_VOIDERR() if (errorp(VoidErr) && !oflag_tst(VoidErr, CAUGHT)) { object *t = VoidErr; VoidErr = nil; return t; }
// propagate returned error -> VoidError
// use instead of PR_ERROR when you can't return an error
#define PR_AS_VOIDERR(x) if (errorp(x) && !oflag_tst(x, CAUGHT)) { VoidErr = x; return 0; }
// propagate VoidError -> VoidError
#define VOID_ABORT() if (errorp(VoidErr) && !oflag_tst(VoidErr, CAUGHT)) return
// propagate returned error -> returned error
#define PR_ERR(x) if (errorp(x) && !oflag_tst(x, CAUGHT)) return x
// propagate 2 errors
#define PR_ERR(x,y) PR_ERR(x); PR_ERR(y)

// New error
object *newerror(errorcode code, object *detail) {
    object *o = newobj();
    PR_ERR(o);
    o->type = ERROR;
    o->code = code;
    o->detail = detail;
    return o;
}
