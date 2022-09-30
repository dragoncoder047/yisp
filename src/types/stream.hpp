#include <Print.h>

object *newstream(Print *s) {
    object *o = newobject();
    PR_ERR(o);
    o->type = STREAM;
    o->stream = s;
    return o;
}

class StringReadStream : public Print {
    object *string;
    uint32_t i;
    uint32_t len;
    StringReadStream(object *str) {
        this->string = str;
        this->i = 0;
        this->len = lstrlen(str);
    }
    int available() {
        return this->len - this->i;
    }
    char read() {
        this->i++;
        return this->peek();
    }
    char peek() {
        return nthchar(this->string, this->i);
    }
    int write(char c) {
        return 0;
    }
}

class StringWriteStream : public Print {
    object *string;
    object *tail;
    StringWriteStream() {
        this->string = newemptystring();
        PR_AS_VOIDERR(this->string);
        this->tail = this->string;
    }
    int available() {
        return 0;
    }
    char read() {
        return -1;
    }
    char peek() {
        return -1;
    }
    int write(char c) {
        buildstring(c, &(this->tail));
    }
}