#ifndef oct_function
#define oct_function

#include "oct_type_pointers.h"

typedef struct oct_Function {
	oct_BType returnType;
	oct_OABType paramTypes;
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

// ... expects a null-terminated array of oct_BType
oct_Bool _oct_Function_construct(struct oct_Context* ctx, oct_BType fn, const char* name, oct_BType retType, ...);

#endif
