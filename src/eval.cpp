
#include "eval.h"
#include "types/ystring.h"
#include "types/ysymbol.h"
#include "types/ycons.h"
#include "types/yerror.h"
#include "cmp.h"

yobj *y_envlookup(yisp_ctx *y, yobj *key, yobj *env) {
    for (; !nullp(env); env = cdr(env)) {
        if (y_eq(key, car(env))) return cdr(env);
    }
    return yerror_frommessage(y, "undefined", NULL, key);
}

yobj *y_eval(yisp_ctx *y, yobj *form, yobj *env) {
    EVAL:
    yield();
    // If the form is an error that hasn't been caught already, bail.
    Y_RETURN_ON_ERROR(form);
    Y_RETURN_ON_NULL(form);
    // If there is not enough space, run a garbage collection. If there still is not enough space, return nil.
    if (y->free_space < 40) ygc_collect(y);
    if (y->free_space < 5) return NULL;
    // If it is a number, complex, error, sting, char, etc (literal), return it.
    if (nullp(form)) return NULL;
    switch (form->type) {
        case FLOAT:
        case INTEGER:
        case ERROR:
        case ERRORDETAILS:
        case STRING:
        case CHARACTER:
        case CFUNCTION:
        case STREAM:
        case STREAMINNER:
        case ARRAY:
            return form;
        case SYMBOL:
            if (keywordp(form)) return form;
            else {
                // Look up the symbol in the environment.
                return y_envlookup(y, form, env);
            }
        case TOKEN:
            return yerror_frommessage(y, "can't eval a token");
    }
    yobj *fun;
    yobj *args;
    // It's a list.
    if (improperp(form)) return yerror_frommessage(y, "problem evaling", NULL, form);
    fun = first(form);
    args = cdr(form);
    // If first is a symbol, eval it in the environment. If it is undefined, bail (undefined).
    if (symbolp(fun)) {
        fun = y_eval(fun, env);
        Y_RETURN_ON_ERROR(fun);
    }
    // Now the first is either a lambda, a lambda macro, or a C function, or a C macro.
    if (cfunp(fun) || (listp(fun) && symbolp(first(fun)) && (ystr_isbuffer(first(fun), "lambda") || ystr_isbuffer(first(fun), "macro")))) {
        /* noop */;
    }
    else {
        // If it isn't one of those, try swapping the first two elements of the list. If that doesn't work, bail (not a function).
        if (ycons_listlength(args) >= 2) {
            fun = first(args);
            if (cfunp(fun) || (listp(fun) && symbolp(first(fun)) && (ystr_isbuffer(first(fun), "lambda") || ystr_isbuffer(first(fun), "macro")))) {
                /* noop */;
            } else {
                return yerror_frommessage(y, "not a function", NULL, first(form));
            }
            args = y_cons(y, first(form), cdr(args));
        }
    }
    return yerror_frommessage(y, "TODO");
    // If it is a lambda or lambda macro literal (not an actual lambda in function position), turn it into a closure. return cons(intern("closure"), cons(dupenv(env), args))
    // If it isn't a macro, EVAL the arguments.
    // If it's a C function or C macro, checkargs, and call it. If the TAILCALL flag is set, go back to 1 and eval the return value.
    // If it is a lambda macro or lambda in function position, call closure(TCstart, function, args, &env) and go back to 1 for a tail call.
    // If it's a bound closure created in step 5, call the closure closure(TCstart, function->cdr, args, &env)
    // Return the result.
    return form;
}
