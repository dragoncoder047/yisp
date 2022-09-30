// new empty string
object *newemptystring() {
    object *obj = newobj();
    PR_ERR(obj);
    obj->type = STRING;
    obj->next = nil;
    obj->chars = 0;
    return obj;
}

// makes a string out of a buffer. If esc is true, \'s are ignored.
object *newstring(char *buffer, bool esc = true) {
    object *obj = newemptystring();
    PR_ERR(obj);
    object *tail = obj;
    while (true) {
        char ch = *buffer++;
        if (ch == '\0') break;
        if (esc && ch == '\\') ch = *buffer++;
        buildstring(ch, &tail);
        PR_VOIDERR();
    }
    return obj;
}

// tacks the character onto the end of the object pointed to by tail
void buildstring(char ch, object **tail) {
    object *nn;
    if ((*tail)->chars & 0xFFFFFF == 0) {
        (*tail)->chars |= ch << 16;
        return;
    } else if ((*tail)->chars & 0xFFFF == 0) {
        (*tail)->chars |= ch << 8;
        return;
    } else if ((*tail)->chars & 0xFF == 0) {
        (*tail)->chars |= ch;
        return;
    } else {
        nn = newemptystring();
        PR_AS_VOIDERR(nn);
        (*tail)->next = nn;
        nn->next = nil;
        nn->chars = ch << 24;
        *tail = nn;
    }
}

// duplicates a string
object *dupstring(object *str) {
    object *dup = nil;
    object *tail = dup;
    object *n;
    while (str != nil) {
        n = newemptystring();
        PR_ERR(n);
        n->chars = str->chars;
        if (dup == nil) {
            dup = n;
        } else {
            tail->next = n;
        }
        tail = n;
        str = str->next;
    }
    return dup;
}

// read from stream
object *readstring(char delim, object *str) {
    if (!streamp(str)) THROW_ERR(NOTASTREAM, str);
    Stream *s = str->stream;
    object *obj = newemptystring();
    PR_ERR(obj);
    object *tail = obj;
    char ch = s->read();
    if (ch == -1) return nil;
    while ((ch != delim) && (ch != -1)) {
        if (ch == '\\') ch = s->read();
        buildstring(ch, &tail);
        PR_VOIDERR();
        ch = s->read();
    }
    return obj;
}

// lisp string length
uint32_t lstrlen(object *form) {
    if (!stringp(form)) THROW_VOIDERR(NOTASTRING, form)
    uint32_t length = 0;
    while (form != nil) {
        uint32_t chars = form->chars;
        for (int i = 24; i >= 0; i -= 8) {
            if (chars >> i & 0xFF) length++;
        }
        form = form->next;
    }
    return length;
}

// get Nth char of string
char nthchar(object *string, uint32_t n) {
    if (!stringp(string)) THROW_VOIDERR(NOTASTRING, string)
    uint32_t top = n >> 2;
    uint8_t b = (3 - (n & 3)) << 3;
    for (int i = 0; i < top; i++) {
        if (string == nil) return -1;
        string = string->next;
    }
    if (string == nil) return -1;
    return (string->chars >> b) & 0xFF;
}

// compares object to characters in buffer
bool eqstrbuf(object *arg, char *buffer) {
    int i = 0;
    while (!(arg == nil && buffer[i] == 0)) {
        if (arg == nil || buffer[i] == 0 || arg->chars != (buffer[i]<<24 | buffer[i+1]<<16 | buffer[i+2]<<8 | buffer[i+3])) return false;
        arg = arg->next;
        i += 4;
    }
    return true;
}

