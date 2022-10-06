# TODO

* [ ] Add complex type `#C(n n)`
* [ ] Rename `object` to `yobj`
* [ ] Use a `template<int size>` for `WORKSPACESIZE`
* [ ] Put all global variables in a struct like tinyScheme
* [ ] Change `fn_ptr_type` signature to `yobj *(*y_fun)(yisp, yobj *, yobj *)`
* [ ] No `OOMError`, simply return `nil` on out-of-memory
* [ ] Change error macros to be obvious `Y_RETURN_ON_NIL`, `Y_ERROR`
* [ ] Functions that don't create new cells never cause an error - return -1 or `nil` on bad data only
* [ ] Add functionality to eval a C `char*` string with a C++ lambda `gfun_t`
* [ ] Add TOKEN type for reader
* [ ] Have `if` ignore `else` as a third parameter, and then defer to fourth.
* [ ] Have generic `def` that can accept `defun`, `defvar`, `define` (Scheme), etc.

## Notes

**Bolded** indicates stuff that I think is unique to Yisp and not just Lisp in particular.

### EVAL algorithm

Given a form and env:

1. If the form is an error that hasn't been caught already, bail.
2. If there is not enough space, run a garbage collection. If there still is not enough space, return `nil`.
3. If it is a number, complex, error, sting, char, etc (literal), return it.
4. It's a list. If first is a symbol, eval it in the environment. If it is undefined, bail (undefined).
5. Now the first is either a lambda, a lambda macro, or a C function, or a C macro. If it isn't one of those, **try swapping the first two elements of the list. If that doesn't work,** bail (not a function).
6. If it is a lambda or lambda macro literal (not an actual lambda in function position), turn it into a closure. `return cons(intern("closure"), cons(dupenv(env), args))`
7. If it isn't a macro, EVAL the arguments.
8. If it's a C function or C macro, checkargs, and call it. If the TAILCALL flag is set, go back to 1 and eval the return value.
9. If it is a lambda macro or lambda in function position, call `closure(TCstart, function, args, &env)` and go back to 1 for a tail call.
10. If it's a bound closure created in step 5, call the closure `closure(TCstart, function->cdr, args, &env)`
11. Return the result.

### CLOSURE algorithm

Given tailcall, function closed, args, and pointer to env:

1. state is stored in car and function in cdr of function.
2. If tailcall is true, drop env pairs until hit a `nil` pair.
3. Add the state vars to the top of the env.
4. ADDARGS to env.
5. `progn` the body and return the tail call.

### ADDARGS algorithm

Given param lambda list, and actual args list:

1. Step through the lists simultaneously.
2. If the current param is the symbol `&optional`, set optional=true.
3. If the current param is the symbol `&key`, do ADDKEYS with remaining params and args, and stop.
4. If the current param is a list and optional=false, bail (invalid argument).
5. If the current param is a list and optional=true, push `cons(first(param), arg)` if arg is not `nil`, else `cons(first(param), second(param))`.
6. If the current param is not a list, push `cons(param, arg)` to env, unless arg is `nil` and optional=false, then bail (too few args).
7. If there are still args left, bail (too many args).

### ADDKEYS algorithm

Given remaining params and actual args:

1. Go through the args list:
2. Expect the first to be a keyword. If it is not, bail (not a keyword).
3. Go through the params list, looking for the un-`:` version of the keyword.
4. If the specified keyword is not in the params list, bail (no such keyword argument).
5. Push `cons(param, second(args))` to the env.
6. `args = cddr(args)`
7. splice the used param out.

### READ algorithm

Given an argument:

1. If the argument is a C string, make it into a stream with a C++ lambda `gfun_t`.
2. READFIRST form the stream.
3. If the item is an open paren, `({[`, READREST until the corresponding close paren. If it is a square bracket, cons `list` onto the start, and if it is curly bracket, cons `progn` onto the start.
4. If the item is a close paren `)]}`, bail (too many close parens).
5. If it is one of `` ` ``, `'`, `,`, `,@`, cons the appropriate macro (`quasiquote`, `quote`, `unquote`, `unquote-splicing`) onto the start of it and the next READ.
6. If it is period token `.`, return next READ.
7. Otherwise, just return the item.

### READFIRST algorithm

Given a stream:

1. Strip space and comments from the front of the stream.
2. If EOF return `nil`.
3. If char is a bracket `{[()]}`, return corresponding bracket token.
4. If char is a single quote or backtick, return corresponding token.
5. If it is a comma, lookahead one, and if it is an @ return UNQUOTESPLICING, else backup and return UNQUOTE.
6. If it is a double quote, READSTRING until double quote.
7. Put the first character in the buffer.
8. If char is a minus, set sign = -1.
9. If char is a dot, look at next, and if it is a space, return DOT token, else set float=true.
10. If char is a pound sign, parse a reader macro:
    1. Look at next character after pound sign. Force to upper case. `ch &= ~0x20`
    2. If it is a backslash, lookahead, and if it is a space or paren, return a character object. Else backup.
    3. If it is a pipe: Read until you get pipe-hash, and then return REAFIRST.
    4. If it is B, O, or X, set base to 2, 8, or 16.
    5. If it is a single quote, just return READFIRST. Ignoring Lisp `#'` macro.
    6. If it is not anyhing above, bail (invalid after `#`).
11. <https://github.com/dragoncoder047/ulisp-esp32/blob/f66f87a/ulisp-esp32.ino#L7172-L7215>
12. return a symbol of the buffer. <https://github.com/dragoncoder047/ulisp-esp32/blob/f66f87a/ulisp-esp32.ino#L7217-L7230>

### READREST algorithm

Given a stream and paren to stop at:

1. get READFIRST from stream.
2. Init head, tail, both `nil`.
3. Rest: See <https://github.com/dragoncoder047/ulisp-esp32/blob/f66f87a/ulisp-esp32.ino#L7241-L7265>
4. Make this take a paren to stop at, and corresponding if cases to handle square and curly brackets.

### READSTRING algorithm

<https://github.com/dragoncoder047/ulisp-esp32/blob/f66f87a/ulisp-esp32.ino#L1281-L1291>, but also make it handle controlcode style escapes after `\`.

## Syntax

Everything on one line is treated as in parenthesis. Unless the line ends in `~`, in which case it is expected an indent and the form continues automagically until indent returns to equal to original. Use indent stack like Python.

```yisp
princ "hello\Newlineworld"
def myVar 1
def myFun (a b c) ~
    myVar := (a + b)
    princ (a + b c myVar)
myFun 1 2 3
myFun 4 5 6
myFun -7 -8 -9
#| a list |# [1 2 3]
#| a progn |# {princ 'foo}
```
