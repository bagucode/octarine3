#include "oct_readable.h"
#include "oct_readable_pointers.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

oct_Bool _oct_Readable_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Readable;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_Readable);
	result = oct_OABType_alloc(ctx, 5, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.I32;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.F32;
	t->variadicType.types.ptr->data[2].ptr = ctx->rt->builtInTypes.String;
	t->variadicType.types.ptr->data[3].ptr = ctx->rt->builtInTypes.Symbol;
	t->variadicType.types.ptr->data[4].ptr = ctx->rt->builtInTypes.ReadableList;
	return oct_True;
}

oct_Bool _oct_OReadable_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OReadable->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OReadable->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OReadable->pointerType.type.ptr = ctx->rt->builtInTypes.Readable;
	return oct_True;
}

oct_Bool _oct_OReadableOption_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OReadableOption->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OReadableOption->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OReadableOption->pointerType.type.ptr = ctx->rt->builtInTypes.OReadable;
	return oct_True;
}
