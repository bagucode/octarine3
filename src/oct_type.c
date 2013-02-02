#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

// Private

oct_Bool _oct_Type_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Type;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_Type);
	result = oct_OABType_alloc(ctx, 8, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.ProtoType;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.TemplateType;
	t->variadicType.types.ptr->data[2].ptr = ctx->rt->builtInTypes.VariadicType;
	t->variadicType.types.ptr->data[3].ptr = ctx->rt->builtInTypes.StructType;
	t->variadicType.types.ptr->data[4].ptr = ctx->rt->builtInTypes.ArrayType;
	t->variadicType.types.ptr->data[5].ptr = ctx->rt->builtInTypes.FixedSizeArrayType;
	t->variadicType.types.ptr->data[6].ptr = ctx->rt->builtInTypes.PointerType;
	t->variadicType.types.ptr->data[7].ptr = ctx->rt->builtInTypes.InterfaceType;
	return oct_True;
}

oct_Bool _oct_BType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BType->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BType->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BType->pointerType.type.ptr = ctx->rt->builtInTypes.Type;
	return oct_True;
}

oct_Bool _oct_ABType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.ABType->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ABType->arrayType.elementType.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}

oct_Bool _oct_OABType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OABType->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OABType->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OABType->pointerType.type.ptr = ctx->rt->builtInTypes.ABType;
	return oct_True;
}

// Public

oct_Bool oct_OABType_alloc(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result) {
	oct_Uword i;
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_ABType) + (sizeof(oct_BType) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// Initialize all to Nothing
	for(i = 0; i < size; ++i) {
		out_result->ptr->data[i].ptr = ctx->rt->builtInTypes.Nothing;
	}
	return oct_True;
}
