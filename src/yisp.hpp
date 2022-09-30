#include <Arduino.h>

// The max number of Lisp cells.
#define WORKSPACESIZE 16384

// ESP32 occasionally doesn't have this defined. Fudge it using the DAC.
#if !defined(analogWrite)
  #define analogWrite(x,y) dacWrite((x), (y))
#endif

// TODO: include everything
#include "alloc.hpp"

object Workspace[WORKSPACESIZE];

object *GCStack;
object *tee;
object *apply (object *function, object *args, object **env);
char nthchar (object *string, int n);
