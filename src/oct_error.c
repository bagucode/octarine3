#include "oct_error.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

oct_Bool _oct_Error_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Error;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_Error);
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_Error, message);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OString;
	return oct_True;
}

oct_Bool _oct_OError_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OError->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OError->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OError->pointerType.type.ptr = ctx->rt->builtInTypes.Error;
	return oct_True;
}

oct_Bool _oct_ErrorOption_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ErrorOption;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_ErrorOption);
	result = oct_OABType_alloc(ctx, 2, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.Nothing;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.OError;
	return oct_True;
}

oct_Bool oct_Error_ctor(struct oct_Context* ctx, oct_OString message, oct_Error* err) {
	err->message.ptr = message.ptr;
	return oct_True;
}

oct_Bool oct_Error_createOwned(struct oct_Context* ctx, oct_OString message, oct_OError* out_err) {
	if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_Error), (void**)&out_err->ptr)) {
		return oct_False;
	}
	out_err->ptr->message.ptr = message.ptr;
	return oct_True;
}

oct_Bool oct_Error_destroyOwned(struct oct_Context* ctx, oct_OError err) {
	oct_String_destroyOwned(ctx, err.ptr->message);
	return oct_ExchangeHeap_free(ctx, err.ptr);
}

