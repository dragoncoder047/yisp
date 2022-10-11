object *GCStack; // all stuff gets pushed to this to prevent GC's

#define mark(x) oflag_set((x), GCMARK)
#define unmark(x) oflag_clr((x), GCMARK)
#define marked(x) oflag_tst((x), GCMARK)

void markobject(object *o) {
    MARK:
    if (o == nil) return;
    if (marked(o)) return;
    mark(obj);
    switch(o->type) {
        case CONS:
        case ARRAY:
            markobject(car(o));
            for (object *d = cdr(o); d != nil; d = cdr(d)) markobject(d);
            break;
        case STRING:
            for (object *d = o->next; d != nil; d = d->next) markobject(d);
            break;
        case ERROR:
            markobject(o->detail);
    }
}

void sweep() {
    Freelist = nil;
    Freespace = 0;
    for (int i = WORKSPACESIZE - 1; i >= 0; i--) {
        object *obj = &Workspace[i];
        if (!marked(obj)) freeobj(obj);
        else unmark(obj);
    }
}

void gc(object *args, object *env) {
    markobject(args);
    markobject(env);
    markobject(GCStack);
    markobject(GlobalEnv);
    sweep();
}