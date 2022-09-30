int64_t checkinteger(object *obj) {
    if (!integerp(obj)) THROW_VOIDERR(NOTANINT, obj);
    return obj->intnum;
}