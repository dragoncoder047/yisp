// makes a new object
object *newobj() {
    if (Freespace == 0) return OOMError;
    object *temp = Freelist;
    Freelist = cdr(Freelist);
    Freespace--;
    car(temp) = nil;
    cdr(temp) = nil;
    return temp;
}

// puts the object into the Freelist
inline void freeobj(object *obj) {
    car(obj) = NULL;
    cdr(obj) = Freelist;
    obj->type = UNASSIGNED;
    obj->objflags = 0;
    Freelist = obj;
    Freespace++;
}
