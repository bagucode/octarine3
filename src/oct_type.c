#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

// Private

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Type_init(oct_Context* ctx) {

	// Type
	oct_BType t = ctx->rt->builtInTypes.Type;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_Type);
	CHECK(oct_ABType_createOwned(ctx, 7, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Prototype;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.Variadic;
	t.ptr->variadicType.types.ptr->data[2] = ctx->rt->builtInTypes.Struct;
	t.ptr->variadicType.types.ptr->data[3] = ctx->rt->builtInTypes.Array;
	t.ptr->variadicType.types.ptr->data[4] = ctx->rt->builtInTypes.FixedSizeArray;
	t.ptr->variadicType.types.ptr->data[5] = ctx->rt->builtInTypes.Pointer;
	t.ptr->variadicType.types.ptr->data[6] = ctx->rt->builtInTypes.Protocol;

	// BType
	ctx->rt->builtInTypes.BType.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BType.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BType.ptr->pointerType.type = ctx->rt->builtInTypes.Type;

	// ABType
	ctx->rt->builtInTypes.ABType.ptr->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ABType.ptr->arrayType.elementType = ctx->rt->builtInTypes.BType;

	// OABType
	ctx->rt->builtInTypes.OABType.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OABType.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OABType.ptr->pointerType.type = ctx->rt->builtInTypes.ABType;

	return oct_True;
}

// Public

oct_Bool oct_ABType_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result) {
	oct_Uword i;
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_ABType) + (sizeof(oct_BType) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// Initialize all to Nothing
	for(i = 0; i < size; ++i) {
		out_result->ptr->data[i] = ctx->rt->builtInTypes.Nothing;
	}
	return oct_True;
}






