#ifndef oct_compiler
#define oct_compiler

#include "oct_object.h"
#include "oct_any.h"

// Public

struct oct_Context;

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result);

#endif
