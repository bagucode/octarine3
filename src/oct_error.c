#include "oct_error.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Error_init(struct oct_Context* ctx) {
	oct_BType t = ctx->rt->builtInTypes.Error;

	// Error
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Error);
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Error, message);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OString;

	// OError
	ctx->rt->builtInTypes.OError.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OError.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OError.ptr->pointerType.type = ctx->rt->builtInTypes.Error;

	// OErrorOption
	t = ctx->rt->builtInTypes.OErrorOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OErrorOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OError;

	return oct_True;
}

oct_Bool oct_Error_createOwned(struct oct_Context* ctx, oct_OString message, oct_OError* out_err) {
	CHECK(OCT_ALLOCRAW(sizeof(oct_Error), (void**)&out_err->ptr, "oct_Error_createOwned"));
	out_err->ptr->message.ptr = message.ptr;
	return oct_True;
}

oct_Bool oct_Error_destroyOwned(struct oct_Context* ctx, oct_OError err) {
	oct_String_destroyOwned(ctx, err.ptr->message);
	return OCT_FREE(err.ptr);
}

