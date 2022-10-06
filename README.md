# Yisp

A Lisp-ish language implemented as a header-only Arduino library. Currently under development- incomplete as of right now.<!--Tested on an ESP32.-->

Inspired by (and most code borrowed from) [@technoblogy/ulisp-esp](https://github.com/technoblogy/ulisp-esp).

Differences:

* Instead of using `setjmp`/`longjmp` to handle errors like uLisp, Yisp simply returns them from a function. An "uncaught error" is simply an object with a type of ERROR and the CAUGHT flag cleared. Error-propagating macros look for this and return the error early if it is not set. This allows Yisp to be used in multi-threaded code.
* Instead of saving state in global variables, Yisp saves its state in a struct. This allows there to be two environments running simultaneously, but independent of each other.
* (*hopefully*) Yisp will allow a more flexible syntax than Lisp that is really heavy on the parenthesis. *Hasn't been implemented yet, but it is planned.*
* (*hopefully*) Yisp will allow pseudo-infix operators by allowing the function and first argument to be swapped, i.e. `(x + y)` instead of `(+ x y)`. *Hasn't been implemented yet.*
