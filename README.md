# Yisp

A Lisp-ish language implemented as a header-only Arduino library. Currently under development- incomplete as of right now.<!--Tested on an ESP32.-->

Inspired by (and most code borrowed from) [@technoblogy/ulisp-esp](https://github.com/technoblogy/ulisp-esp).

The main difference from uLisp is that instead of using `setjmp`/`longjmp` to handle errors, Yisp simply returns them from a function. An "uncaught error" is simply an object with a type of ERROR and the CAUGHT flag cleared. Error-propagating macros look for this and return the error early if it is not set. This allows Yisp to be used in multi-threaded code.
