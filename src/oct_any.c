#include "oct_any.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_type_pointers.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Any_init(struct oct_Context* ctx) {
	// Any
	oct_CType t = ctx->rt->builtInTypes.Any;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_Any);
	CHECK(oct_ACType_createOwned(ctx, 3, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OObject;
	t.ptr->variadicType.types.ptr->data[2] = ctx->rt->builtInTypes.BObject;
	return oct_True;
}

