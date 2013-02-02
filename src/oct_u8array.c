#include "oct_u8array.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <memory.h>

// Private

oct_Bool _oct_AU8_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.AU8->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AU8->arrayType.elementType.ptr = ctx->rt->builtInTypes.U8;
	return oct_True;
}

oct_Bool _oct_OAU8_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OAU8->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAU8->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAU8->pointerType.type.ptr = ctx->rt->builtInTypes.AU8;
	return oct_True;
}

// Public

oct_Bool oct_OAU8_alloc(struct oct_Context* ctx, oct_Uword size, oct_OAU8* out_result) {
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_AU8) + (sizeof(oct_U8) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// "construct"
	memset(&out_result->ptr->data[0], 0, (sizeof(oct_U8) * size));
	return oct_True;
}

