#include "oct_chararray.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <memory.h>

oct_Bool _oct_AChar_init(struct oct_Context* ctx) {
	// AChar
	ctx->rt->builtInTypes.AChar.ptr->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AChar.ptr->arrayType.elementType = ctx->rt->builtInTypes.Char;

	// OAChar
	ctx->rt->builtInTypes.OAChar.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAChar.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAChar.ptr->pointerType.type = ctx->rt->builtInTypes.AChar;

	return oct_True;
}

// Public

oct_Bool oct_AChar_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAChar* out_result) {
	oct_Uword allocSize = sizeof(oct_AChar) + (sizeof(oct_Char) * size);
	if(!OCT_ALLOCOWNED(allocSize, (void**)&out_result->ptr, "oct_AChar_createOwned")) {
		return oct_False;
	}
	out_result->ptr->size = size;
	// "construct"
	memset(&out_result->ptr->data[0], 0, (sizeof(oct_Char) * size));
	return oct_True;
}
