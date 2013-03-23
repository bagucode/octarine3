#ifndef oct_function
#define oct_function

#include "oct_self.h"
#include "oct_type_pointers.h"

typedef struct oct_Function {
	oct_OABType paramTypes;
	oct_OABType returnTypes;
} oct_Function;

typedef struct oct_BFunction {
	oct_Function* ptr;
} oct_BFunction;

typedef struct oct_ABFunction {
	oct_Uword size;
	oct_BFunction data[];
} oct_ABFunction;

typedef struct oct_OABFunction {
	oct_ABFunction* ptr;
} oct_OABFunction;

struct oct_Context;

oct_Bool _oct_Function_init(struct oct_Context* ctx);

oct_Bool oct_Function_dtor(struct oct_Context* ctx, oct_BSelf self);
oct_Bool oct_ABFunction_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OABFunction* out_result);

#endif
