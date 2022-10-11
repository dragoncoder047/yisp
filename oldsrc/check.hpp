#include <Print.h>

int64_t checkinteger(object *obj) {
    if (!integerp(obj)) THROW_VOIDERR(NOTANINT, obj);
    return obj->intnum;
}

double checkintfloat(object *obj){
    if (integerp(obj)) return obj->integer;
    if (!floatp(obj)) THROW_VOIDERR(NOTAFLOAT, obj);
    return obj->floatnum;
}

object *checkstring(object *obj) {
    if (!stringp(obj)) THROW_ERR(NOTASTRING, obj);
    return obj;
}

Print *checkstream(object *obj) {
    if (!streamp(obj)) THROW_VOIDERR(NOTASTREAM, obj);
    return obj->stream;
}
