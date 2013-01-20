#include "oct_function.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"

#include <stddef.h>

oct_Bool _oct_Function_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Function;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.size = sizeof(oct_Function);
	t->structType.alignment = 0;
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_Function, returnType);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.BType;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_Function, argTypes);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.OABType;
	return oct_True;
}

oct_Bool _oct_BFunction_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BFunction->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BFunction->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BFunction->pointerType.type.ptr = ctx->rt->builtInTypes.Function;
	return oct_True;
}

oct_Bool _oct_ABFunction_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BFunction->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.BFunction->arrayType.elementType.ptr = ctx->rt->builtInTypes.BFunction;
	return oct_True;
}

oct_Bool _oct_OABFunction_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BFunction->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BFunction->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.BFunction->pointerType.type.ptr = ctx->rt->builtInTypes.ABFunction;
	return oct_True;
}
