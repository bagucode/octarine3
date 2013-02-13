#include "oct_chararray.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <memory.h>

// Private

oct_Bool _oct_AChar_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.AChar->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AChar->arrayType.elementType.ptr = ctx->rt->builtInTypes.Char;
	return oct_True;
}

oct_Bool _oct_OAChar_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OAChar->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAChar->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAChar->pointerType.type.ptr = ctx->rt->builtInTypes.AChar;
	return oct_True;
}

// Public

oct_Bool oct_AChar_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAChar* out_result) {
	oct_BType bt;
	bt.ptr = ctx->rt->builtInTypes.AChar;
	if(!oct_ExchangeHeap_allocArray(ctx, bt, size, (void**)&out_result->ptr)) {
		return oct_False;
	}
	out_result->ptr->size = size;
	// "construct"
	memset(&out_result->ptr->data[0], 0, (sizeof(oct_Char) * size));
	return oct_True;
}

