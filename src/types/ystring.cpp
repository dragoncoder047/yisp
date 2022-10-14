
#include "ystring.h"

// makes a string out of a buffer.
yobj *ystring_frombuffer(yisp_ctx *y, char *buffer) {
    yobj *obj = y_newobj(y, STRING);
    Y_RETURN_ON_NULL(obj);
    yobj *tail = obj;
    while (true) {
        char ch = *buffer++;
        if (ch == '\0') break;
        bool err = _y_buildstring(ch, &tail);
        if (err) return NULL;
    }
    return obj;
}

// tacks the character onto the end of the yobj pointed to by tail
bool _y_buildstring(yisp_ctx *y, char ch, yobj **tail) {
    yobj *nn;
    if ((*tail)->chars & 0xFFFFFF == 0) {
        (*tail)->chars |= ch << 16;
        return false;
    } else if ((*tail)->chars & 0xFFFF == 0) {
        (*tail)->chars |= ch << 8;
        return false;
    } else if ((*tail)->chars & 0xFF == 0) {
        (*tail)->chars |= ch;
        return false;
    } else {
        nn = y_newobj(y, STRING);
        if (nullp(nn)) return true;
        (*tail)->next = nn;
        nn->chars = ch << 24;
        *tail = nn;
    }
}

// duplicates a string
yobj *ystr_copy(yisp_ctx *y, yobj *str) {
    Y_RETURN_ON_NULL(str);
    Y_RETURN_ON_ERROR(str);
    yobj *dup = y_newobj(y, STRING);
    yobj *tail = dup;
    yobj *n;
    Y_RETURN_ON_NULL(dup);
    tail->chars = str->chars;
    str = str->next;
    while (str != NULL) {
        n = y_newobj(y, STRING);
        Y_RETURN_ON_NULL(n);
        n->chars = str->chars;
        tail->next = n;
        tail = n;
        str = str->next;
    }
    return dup;
}

// read from stream
yobj *ystr_readstring(yisp_ctx *y, char delim, yobj *str) {
    if (!streamp(str)) return NULL;
    gfun_t g = str->funs->gfun;
    yobj *obj = y_newobj(y, STRING);
    Y_RETURN_IF_NULL(obj);
    yobj *tail = obj;
    char ch = g();
    if (ch == -1) return NULL;
    while (ch != delim && ch != -1) {
        if (ch == '\\') {
            // TODO: multichar escapes
            ch = g();
        }
        bool err = _y_buildstring(ch, &tail);
        if (err) return NULL;
        ch = g();
    }
    return obj;
}

// lisp string length
size_t ystr_length(yobj *s) {
    if (!stringp(s)) return -1;
    size_t length = 0;
    while (s != NULL) {
        uint32_t chars = s->chars;
        for (int i = 24; i >= 0; i -= 8) {
            if (chars >> i & 0xFF) length++;
        }
        s = s->next;
    }
    return length;
}

// get Nth char of string
char ystr_nthchar(yobj *string, size_t n) {
    if (!stringp(string)) return -1;
    uint32_t top = n >> 2;
    uint8_t b = (3 - (n & 3)) << 3;
    for (int i = 0; i < top; i++) {
        if (string == NULL) return -1;
        string = string->next;
    }
    if (string == NULL) return -1;
    return (string->chars >> b) & 0xFF;
}

// compares object to characters in buffer
bool ystr_isbuffer(yobj *s, char *buffer) {
    int i = 0;
    while (!(s == NULL && buffer[i] == 0)) {
        if (s == NULL || buffer[i] == 0 || s->chars != (buffer[i]<<24 | buffer[i+1]<<16 | buffer[i+2]<<8 | buffer[i+3])) return false;
        s = s->next;
        i += 4;
    }
    return true;
}

size_t ystr_tobuffer(yobj *s, char *buffer, size_t maxlen) {
    size_t i = 0;
    while (s !== NULL) {
        for (uint8_t x = 24, x > 0; x -= 8) {
            char c = (s->chars & (0xFF << x)) >> x;
            buffer[i++] = c;
            if (c == '\0' || i == maxlen) return i;
        }
        s = s->next;
    }
    return i;
}
