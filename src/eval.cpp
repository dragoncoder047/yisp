
#include "eval.h"
#include "types/ystring.h"
#include "types/ysymbol.h"
#include "types/ycons.h"
#include "types/yerror.h"
#include "cmp.h"
#include "functions/progn.h"

yobj *y_envlookup(yisp_ctx *y, yobj *key, yobj *env) {
    for (; !nullp(env); env = cdr(env)) {
        if (nullp(env)) break;
        yobj *pair = car(env);
        if (nullp(pair)) continue;
        if (y_eq(key, car(pair))) return cdr(pair);
    }
    return yerror_frommessage(y, "undefined", NULL, key);
}

yobj *y_eval(yisp_ctx *y, yobj *form, yobj *env) {
    bool tail = false;
    EVAL:
    yield();
    // If the form is an error that hasn't been caught already, bail.
    Y_RETURN_ON_ERROR(form);
    Y_RETURN_ON_NULL(form);
    // If there is not enough space, run a garbage collection. If there still is not enough space, return nil.
    if (y->free_space < 40) ygc_collect(y, form, env);
    if (y->free_space < 5) return NULL;
    // If it is a number, complex, error, sting, char, etc (literal), return it.
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
            return yerror_frommessage(y, "can't eval a token", NULL, form);
    }
    // It's a list.
    if (improperp(form)) return yerror_frommessage(y, "problem evaling", NULL, form);
    yobj *fun = first(form);
    yobj *args = cdr(form);
    if (nullp(fun)) return yerror_frommessage(y, "tried to call nil");
    if (improperp(fun)) return yerror_frommessage(y, "can't eval a dotted pair", NULL, fun);
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
                args = y_cons(y, first(form), cdr(args));
            } else {
                return yerror_frommessage(y, "not a function", NULL, first(form));
            }
        }
    }
    // If it is a lambda or lambda macro literal (not an actual lambda in function position), turn it into a closure. return cons(intern("closure"), cons(dupenv(env), args))
    // -----> this is handled by the macro and lambda macro-functions.

    // If it isn't a macro, EVAL the arguments.
    bool is_macro = cfunp(fun) && ((fun->funinfo >> 8) & MACRO);
    if (!is_macro) {
        is_macro = ystr_isbuffer(first(fun), "macro");
    }
    if (!is_macro) {
        yobj *results = y_cons(y, y_eval(y, first(args), env), NULL);
        Y_RETURN_ON_NULL(results);
        Y_RETURN_ON_ERROR(results);
        yobj *tail = results;
        args = cdr(args);
        push(y, results, y->gc_stack);
        while (!nullp(args)) {
            yobj *temp = y_cons(y, y_eval(y, car(args), env), NULL);
            Y_RETURN_ON_NULL(temp);
            cdr(tail) = temp;
            tail = temp;
            args = cdr(args);
        }
        pop(y->gc_stack);
        args = results;
    }
    // If it's a C function or C macro, checkargs, and call it.
    switch (yfun_checkargs(fun, args)) {
        case TOOMANY:
            return yerror_frommessage(y, "too many arguments", fun, args);
        case TOOFEW:
            return yerror_frommessage(y, "too few arguments", fun, args);
        case NOTAFUNCTION:
            break;
        case OK:
            form = fun->cfun(args, env);
            // If the TAILCALL flag is set, go back to 1 and eval the return value.
            if ((fun->funinfo >> 8) & TAIL) goto EVAL;
            else return form;
    }
    // If it is a lambda macro or lambda in function position, call closure(TCstart, function, args, &env) and go back to 1 for a tail call.
    // If it's a bound closure created in step 5, call the closure closure(TCstart, function->cdr, args, &env)
    bool tail_start = tail;
    if (consp(fun)) {
        yobj *name = first(fun);
        if (ystr_isbuffer(name, "closure")) fun = cdr(fun);
        if (ystr_isbuffer(name, "lambda") || ystr_isbuffer(name, "macro") || ystr_isbuffer(name, "closure")) {
            form = y_closure(y, tail_start, fun, args, &env);
            Y_RETURN_ON_ERROR(form);
            tail = true;
            goto EVAL;
        } else return yerror_frommessage(y, "can't call a list", NULL, form);
    }
    // Return the result.
    return form;
}

yobj *y_closure(yisp_ctx *y, bool tail, yobj *thunk, yobj *args, yobj **env) {
    // state is stored in car and function in cdr of function.
    yobj *state = first(thunk);
    yobj *params = second(thunk);
    yobj *funbody = third(thunk);
    if (improperp(params)) return yerror_frommessage(y, "params must be a list", NULL, cdr(thunk));
    // If tailcall is true, drop env pairs until hit a nil pair.
    if (tail) {
        if (*env != NULL && car(*env) == NULL) {
            pop(*env);
            while (*env != NULL && car(*env) != NULL) pop(*env);
        } else {
            push(y, NULL, *env);
        }
    }
    // Add the state vars to the top of the env.
    while (consp(state)) {
        yobj *pair = first(state);
        push(y, pair, *env);
        state = cdr(state);
    }
    // Add args
    y_addargs(y, params, args, env);
    Y_IF_ERROR(*env) return *env;
    if (tail) {
        push(y, NULL, *env);
    }
    // progn the body and return the tail call.
    return tf_progn(y, funbody, *env);
}

void y_addargs(yisp_ctx, *y, yobj *params, yobj *args, yobj **env) {
    bool optional = false;
    yobj *param;
    yobj *arg;
    while (params != NULL) {
        // Step through the lists simultaneously.
        param = car(params);
        if (args) arg = car(args);
        if (symbolp(param)) {
            // If the current param is the symbol &optional, set optional=true.
            if (ystr_isbuffer(param, "&optional\0\0\0")) optional = true;
            // If the current param is the symbol &key, do ADDKEYS with remaining params and args, and stop.
            else if (ystr_isbuffer(param, "&key\0\0\0")) {
                y_addkwargs(y, params, args, env);
                return;
            }
            // If the current param is the symbol &rest, expect another param after it, and then assign the remaining args to it, and stop.
            else if (ystr_isbuffer(param, "&rest\0\0\0")) {
                if (nullp(cdr(param))) {
                    *env = yerror_frommessage(y, "&rest needs another param after it");
                    return;
                }
                else {
                    params = cdr(params);
                    param = car(params);
                    arg = args;
                    args = NULL;
                }
            }
            // If the current param is not a list, push cons(param, arg) to env, unless arg is nil and optional=false, then bail (too few args).
            else if (nullp(args) && !optional) {
                *env = yerror_frommessage(y, "too few args");
                return;
            }
        }
        else if (listp(param)) {
            // If the current param is a list and optional=false, bail (invalid argument).
            if (!optional) {
                *env = yerror_frommessage(y, "param with default must come after &optional");
                return;
            }
            // If the current param is a list and optional=true, push cons(first(param), arg) if arg is not nil, else cons(first(param), second(param)).
            else if (!nullp(arg)) {
                if (consp(cdr(param))) arg = second(param);
            }
            param = first(param);
        } else {
            *env = yerror_frommessage(y, "invalid parameter");
            return;
        }
        push(y, y_cons(y, param, arg), *env);
        params = cdr(params);
        if (args) args = cdr(args);
    }
    if (args) {
        *env = yerror_frommessage(y, "too many args");
    }
}

void y_addkwargs(yisp_ctx, *y, yobj *params, yobj *args, yobj **env) {
    if (nullp(params_copy)) {
        **env = NULL;
        return;
    }
    char *buffer[64]; // For cutting off the : on the keyword
    while (args != NULL) {
        // Expect the first to be a keyword. If it is not, bail (not a keyword).
        if (!keywordp(first(args))) {
            *env = yerror_frommessage(y, "expected a keyword here");
            return;
        }
        ystr_tobuffer(first(args), buffer, sizeof(buffer));
        // Go through the params list, looking for the un-: version of the keyword.
        yobj *params_head = params;
        bool found = false;
        while (params_head != NULL) {
            if (!yoflag_tst(car(params_head), KWUSED)) {
                yobj *param = car(params_head);
                if (!symbolp(param)) {
                    *env = yerror_frommessage(y, "invalid keyword parameter", NULL, params);
                    return;
                }
                if (ystr_isbuffer(param, buffer + 1)) { // to cut off :
                    if (ycons_listlen(args) < 2) {
                        *env = yerror_frommessage(y, "need argument after keyword", NULL, param);
                    }
                    // Push cons(param, second(args)) to the env.
                    push(y, y_cons(y, param, second(args)), *env);
                    args = cddr(args);
                    // splice the used param out.
                    yoflag_set(param, KWUSED);
                    found = true;
                }
            }
            params_head = cdr(params_head)
        }
        // If the specified keyword is not in the params list, bail (no such keyword argument).
        if (!found) {
            *env = yerror_frommessage(y, "no such keyword argument", NULL, first(args));
            return;
        }
    }
    // add remaining params as NULL
    // clear USED flag on params
    for (yobj *pc = params; pc != NULL, pc = cdr(pc)) {
        if (pc == NULL) break;
        if (yoflag_tst(car(pc), KWUSED)) yoflag_clr(car(pc), KWUSED);
        else {
            push(y, y_cons(y, car(pc), NULL), *env);
        }
    }
}
