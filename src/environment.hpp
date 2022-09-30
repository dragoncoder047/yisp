object *GlobalEnv;

object *value(object *key, object *env) {
    for (object *pair = env; pair != nil; pair = cdr(pair)) {
        if (eq(car(pair), key)) return cdr(pair);
    }
    return nil;
}
