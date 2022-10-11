object *closure(int tc, object *state, object *function, object *args, object **env) {
    PR_ERR(state, function); PR_ERR(args, *env);
    object *params = first(function);
    function = cdr(function);
    // Dropframe
    if (tc) {
        if (*env != nil && car(*env) == nil) {
            pop(*env);
            while (*env != nil && car(*env) != nil) pop(*env);
        } else push(nil, *env);
    }
    PR_ERR(*env);
    // Push state
    while (state != nil) {
        object *pair = first(state);
        push(pair, *env);
        state = cdr(state);
        PR_ERR(*env);
    }
    // Add arguments to environment
    bool optional = false;
    while (params != nil) {
        object *value;
        object *var = first(params);
        if (symbolp(var) && eqstrbuf(var, "&optional")) optional = true;
        else {
            if (consp(var)) {
                if (!optional) THROW_ERR(MALFORMEDPARAM, var);
                if (args == nil) value = eval(second(var), *env);
                else { value = first(args); args = cdr(args); }
                PR_ERR(value);
                var = first(var);
                if (!symbolp(var)) THROW_ERR(MALFORMEDPARAM, var);
            } else if (!symbolp(var)) {
                THROW_ERR(MALFORMEDPARAM, var);
            } else if (eqstrbuf(var, "&rest")) {
                params = cdr(params);
                var = first(params);
                value = args;
                args = nil;
            } else {
                if (args == nil) {
                    if (optional) value = nil; 
                    else THROW_ERR(TOOFEWARGS);
                } else { value = first(args); args = cdr(args); }
            }
            push(cons(var,value), *env);
            PR_ERR(first(*env));
            // if (trace) { pserial(' '); printobject(value, pserial); }
        }
        params = cdr(params);  
    }
    if (args != nil) THROW_ERR(TOOMANYARGS);
    // if (trace) { pserial(')'); pln(pserial); }
    // Do an implicit progn
    if (tc) push(nil, *env);
    PR_ERR(*env);
    return tf_progn(function, *env);
}

object *apply(object *function, object *args, object *env) {
    PR_ERR(function, args);
    PR_ERR(env);
    if (cfunp(function)) {
        checkargs(function->info, args);
        PR_VOIDERR();
        return function->cfun(args, env);
    }
    if (consp(function) && symbolp(car(function)) && eqstrbuf(car(function), "lambda")) {
        function = cdr(function);
        object *result = closure(0, nil, nil, function, args, &env);
        PR_ERR(result);
        return eval(result, env);
    }
    if (consp(function) && symbolp(car(function)) && eqstrbuf(car(function), "closure")) {
        function = cdr(function);
        object *result = closure(0, 0, car(function), cdr(function), args, &env);
        PR_ERR(result);
        return eval(result, env);
    }
    THROW_ERR(NOTAFUNCTION, function);
}

void checkargs(uint32_t info, object *args) {
    int minmax = info & 0xFF;
    int nargs = listlength(args);
    VOID_ABORT();
    if (nargs < (minmax >> 4)) THROW_VOIDERR(TOOFEWARGS);
    if ((minmax & 0x0F) != 0x0F && nargs > (minmax & 0x0F)) THROW_VOIDERR(TOOMANYARGS);
}

object *eval (object *form, object *env) {
    int TC=0;
    EVAL:
    yield();  // Needed on ESP8266 to avoid Soft WDT Reset or ESP32 for multithreading
    PR_ERR(form, env);
    // Enough space?
    if (Freespace <= WORKSPACESIZE>>4) gc(form, env);
    // Escape
    // if (tstflag(ESCAPE)) { clrflag(ESCAPE); error2(NIL, PSTR("escape!"));}
    // if (!tstflag(NOESC)) testescape();

    // Evaluates to itself?
    if (
        form == nil ||
        stringp(form) ||
        characterp(form) ||
        numberp(form) ||
        keywordp(form) ||
        streamp(form) ||
        cfunp(form)
    ) return form;

    // Bare symbol?
    if (symbolp(form)) {
        object *pair = value(form, env);
        if (pair != nil) return cdr(pair);
        THROW_ERR(NOTDEFINED, form);
    }

    // It's a list.
    object *function = car(form);
    object *args = cdr(form);
    object *unevaled_args = cdr(form);

    if (function == nil) THROW_ERR(NOTAFUNCTION, nil);
    if (!listp(args)) THROW_ERR(INVALIDARGS, args);

    // List starts with a symbol?
    if (symbolp(function)) {
        bool isletstar = eqstrbuf(function, "let*");
        if (eqstrbuf(function, "let") || isletstar) {
            int TCstart = TC;
            if (args == nil) THROW_ERR(TOOFEWARGS);
            object *assigns = first(args);
            if (!listp(assigns)) THROW_ERR(MALFORMEDLIST, assigns);
            object *forms = cdr(args);
            object *newenv = env;
            push(newenv, GCStack);
            while (assigns != nil) {
                object *assign = car(assigns);
                if (!consp(assign)) push(cons(assign,nil), newenv);
                else if (cdr(assign) == nil) push(cons(first(assign), nil), newenv);
                else push(cons(first(assign), eval(second(assign), env)), newenv);
                car(GCStack) = newenv;
                if (isletstar) env = newenv;
                assigns = cdr(assigns);
            }
            env = newenv;
            pop(GCStack);
            form = tf_progn(forms, env);
            TC = TCstart;
            goto EVAL;
        }

        if (eqstrbuf(function, "lambda")) {
            if (env == nil) return form;
            object *envcopy = nil;
            while (env != nil) {
                object *pair = first(env);
                if (pair != nil) push(pair, envcopy);
                env = cdr(env);
            }
            return cons(intern("closure"), cons(envcopy, args));
        }

        if (cfunp(function)) {
            checkargs(function->info, args);
            form = function->cfun(args, env);
            if (funtion->info & TAIL) {
                TC = 1;
                goto EVAL;
            }
        }

        THROW_ERR(NOTAFUNCTION, function);
    }

    // Evaluate the parameters - result in head
    object *fname = car(form);
    int TCstart = TC;
    object *head = cons(eval(fname, env), nil);
    push(head, GCStack); // Don't GC the result list
    object *tail = head;
    form = cdr(form);
    int nargs = 0;

    while (form != nil){
        object *obj = cons(eval(car(form), env), nil);
        cdr(tail) = obj;
        tail = obj;
        form = cdr(form);
        nargs++;
    }

    function = car(head);
    args = cdr(head);

    if (cfunp(function)) {
        checkargs(function->info, args);
        object *result = function->cfun(args, env);
        pop(GCStack);
        return result;
    }
    // lambda literal
    if (consp(function)) {
        symbol_t name = sym(NIL);
        if (!listp(fname)) name = fname->name;
        // TODO here
        if (eqstrbuf(car(function), "lambda")) {
            form = closure(TCstart, function, args, &env);
            pop(GCStack);
            TC = 1;
            goto EVAL;
        }

        if (eqstrbuf(car(function), "closure")) {
            function = cdr(function);
            form = closure(TCstart, function, args, &env);
            pop(GCStack);
            TC = 1;
            goto EVAL;
        }

    }
    if (consp(function) && (eqstrbuf(car(function), "macro"))) {
        // has the form (MACRO (param...) expr...)
        object *definition = cdr(function);
        object *params = car(definition);
        object *body = car(cdr(definition));
        push(unevaled_args, GCStack);
        // Serial.print("Macro: params: ");
        // printobject(params, pserial);
        // Serial.print(" - args: ");
        // printobject(args, pserial);
        // Serial.print(" - body: ");
        // printobject(body, pserial);
        // Serial.println();
        object *newenv = env;
        push(newenv, GCStack);
        object *p;
        object *a;
        for (p = params, a = unevaled_args; p != NULL; p = cdr(p), a = cdr(a)) {
            if (isbuiltin(car(p), AMPREST)) {
                push(cons(car(cdr(p)), a), newenv);
                car(GCStack) = newenv;
                break;
            } else {
                push(cons(car(p), eval(car(a), env)), newenv);
                // push(cons(car(p), car(a)), newenv);
                car(GCStack) = newenv;
            }
        }
        form = eval(body, newenv);
        pop(GCStack);
        pop(GCStack);
        goto EVAL;
    }
    error(NIL, PSTR("illegal function"), fname); return nil;
}