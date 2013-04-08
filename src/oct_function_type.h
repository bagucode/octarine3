#ifndef oct_function_type
#define oct_function_type

#include "oct_function.h"
#include "oct_type_type.h"

// Function
oct_AField _oct_FunctionFields = {
	2, {
		{offsetof(oct_Function, paramTypes), {(oct_Type*)&_oct_CACTypeType}},
		{offsetof(oct_Function, returnTypes), {(oct_Type*)&_oct_CACTypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_FunctionType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_Function), {&_oct_FunctionFields}}
};

// CFunction
OCT_DEF_C_POINTER(CFunction, Function);

// ACFunction
OCT_DEF_ARRAY(ACFunction, CFunction);

// CACFunction
OCT_DEF_C_POINTER(CACFunction, ACFunction);

#endif
