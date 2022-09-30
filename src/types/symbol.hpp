// New symbol
object *newsymbol(char *buffer) {
    object *str = newstring(buffer);
    PR_ERR(str);
    str->type = SYMBOL;
    return str;
}

// Looks up the symbol and creates it only if it is not found.
object *intern(char *buffer) {
  for (int i = 0; i < WORKSPACESIZE; i++) {
    object *obj = &Workspace[i];
    if (symbolp(obj) && eqstrbuf(obj, buffer)) return obj;
  }
  return newsymbol(buffer);
}
