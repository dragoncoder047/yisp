// Main Yisp EVAL algorithm

#ifndef _yisp_eval_h_
#define _yisp_eval_h_
#include "memory.h"

yobj *y_eval(yisp_ctx *y, yobj *form, yobj *env);

#include "eval.cpp"

#endif