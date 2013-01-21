#include "oct_compiler.h"

// Built in forms:
// def    -> establish binding in current namespace
// let    -> establish binding in current frame
// if     -> conditional execution
// do     -> eval multiple forms and return result of last
// quote  -> suppress evaluation
// fn     -> create function (lambda)
// throw? -> throw exception (if used, also need catch and finally?)
// .      -> get value of struct member
// !      -> set value of struct member

// one creation form for each kind of data type...
// struct -> create struct
// ...

// 

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_BReadable readable, oct_Any* out_result) {
}
