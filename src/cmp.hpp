#define same(t, x, y) (x->type == t && y->type == t)

bool eq(object *arg1, object *arg2) {
    if (arg1 == arg2) return true;  // Same object
    if ((arg1 == nil) || (arg2 == nil)) return false;  // Not both values
    if (arg1->type != arg2->type) return false; // Different type
    if (car(arg1) != car(arg2) || cdr(arg1) != cdr(arg2)) return false;  // Different values
    if (symbolp(arg1)) {  // Same symbol
        object *args = cons(arg1, cons(arg2, nil));
        return stringcompare(args, false, false, true);
    }
    return false;
}

bool equal(object *arg1, object *arg2) {
    if (arg1->type != arg2->type) return false; // Different type
    if (eq(arg1, arg2)) return true;
    if (stringp(arg1)) {
        object *args = cons(arg1, cons(arg2, nil));
        return stringcompare(args, false, false, true);
    }
    if (consp(arg1) || arrayp(arg1)) {
        while (arg1 != nil && arg2 != nil) {
            if (!equal(car(arg1), car(arg2))) return false;
            arg1 = cdr(arg1);
            arg2 = cdr(arg2);
        }
        return arg1 == nil && arg2 == nil;
    }
    return false;
}

bool stringcompare(object *args, bool lt, bool gt, bool eq) {
  object *arg1 = checkstring(name, first(args));
  object *arg2 = checkstring(name, second(args));
  arg1 = cdr(arg1);
  arg2 = cdr(arg2);
  while ((arg1 != NULL) || (arg2 != NULL)) {
    if (arg1 == NULL) return lt;
    if (arg2 == NULL) return gt;
    if (arg1->chars < arg2->chars) return lt;
    if (arg1->chars > arg2->chars) return gt;
    arg1 = car(arg1);
    arg2 = car(arg2);
  }
  return eq;
}