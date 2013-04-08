#ifndef oct_variadictype_private
#define oct_variadictype_private

#include "oct_type_pointers.h"

typedef struct oct_VariadicType {
	oct_Uword alignment;
	oct_Uword size;
	oct_CACType types;
} oct_VariadicType;

// Private

struct oct_Context;

oct_Bool _oct_VariadicType_init(struct oct_Context* ctx);

#endif
