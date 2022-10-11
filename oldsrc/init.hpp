uint32_t Freespace = 0;
object *OOMError;
object *tee;

// sets up the workspace and initializes constants
void initworkspace() {
    Freelist = nil;
    for (int i = WORKSPACESIZE - 1; i >= 0; i--) {
        object *obj = &Workspace[i];
        car(obj) = nil;
        cdr(obj) = Freelist;
        Freelist = obj;
        Freespace++;
    }
    OOMError = newerror(OUTOFMEMORY, nil);
}

void initenv() {
    tee = intern("t");
    push(cons(tee, tee), GlobalEnv);
}