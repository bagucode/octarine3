#ifndef oct_function
#define oct_function

#include "oct_generic.h"
#include "oct_type_pointers.h"

typedef struct oct_Function {
	oct_CACType paramTypes;
	oct_CACType returnTypes;
} oct_Function;

typedef struct oct_CFunction {
	oct_Function* ptr;
} oct_CFunction;

typedef struct oct_ACFunction {
	oct_Uword size;
	oct_CFunction data[];
} oct_ACFunction;

typedef struct oct_CACFunction {
	oct_ACFunction* ptr;
} oct_CACFunction;

struct oct_Context;

oct_Bool _oct_Function_init(struct oct_Context* ctx);

//oct_Bool oct_Function_dtor(struct oct_Context* ctx, oct_BGeneric self);
//oct_Bool oct_ACFunction_createOwned(struct oct_Context* ctx, oct_Uword size, oct_CACFunction* out_result);

#endif
