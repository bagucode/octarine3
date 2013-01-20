#ifndef oct_function
#define oct_function

#include "oct_type_pointers.h"

typedef struct oct_Function {
	oct_BType returnType;
	oct_OABType argTypes;
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

oct_Bool _oct_Function_initType(struct oct_Context* ctx);
oct_Bool _oct_BFunction_initType(struct oct_Context* ctx);
oct_Bool _oct_ABFunction_initType(struct oct_Context* ctx);
oct_Bool _oct_OABFunction_initType(struct oct_Context* ctx);

#endif
