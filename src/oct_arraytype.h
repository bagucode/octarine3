#ifndef oct_arraytype_private
#define oct_arraytype_private

#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_ArrayType {
	oct_BType elementType;
} oct_ArrayType;

typedef struct oct_FixedSizeArrayType {
	oct_Uword size;
	oct_BType elementType;
} oct_FixedSizeArrayType;

// Private

struct oct_Context;

oct_Bool _oct_ArrayType_initType(struct oct_Context* ctx);
oct_Bool _oct_FixedSizeArrayType_initType(struct oct_Context* ctx);

#endif
