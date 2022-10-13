// Main Yisp EVAL algorithm

#ifndef _yisp_eval_h_
#define _yisp_eval_h_
#include "memory.h"

yobj *y_envlookup(yisp_ctx *y, yobj *key, yobj *env);
yobj *y_eval(yisp_ctx *y, yobj *form, yobj *env);
yobj *y_closure(yisp_ctx *y, bool tail, yobj *thunk, yobj *args, yobj **env);
void y_addargs(yisp_ctx, *y, yobj *params, yobj *args, yobj **env);
void y_addkwargs(yisp_ctx, *y, yobj *params, yobj *args, yobj **env);

#include "eval.cpp"

#endif