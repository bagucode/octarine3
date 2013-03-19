#include "oct_function.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Function_init(struct oct_Context* ctx) {

	// Function
	oct_BType t = ctx->rt->builtInTypes.Function;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_Function);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Function, returnType);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.BType;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Function, paramTypes);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OABType;

	// BFunction
	t = ctx->rt->builtInTypes.BFunction;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Function;

	// ABFunction
	t = ctx->rt->builtInTypes.ABFunction;
	t.ptr->variant = OCT_TYPE_ARRAY;
	t.ptr->arrayType.elementType = ctx->rt->builtInTypes.BFunction;

	// OABFunction
	t = ctx->rt->builtInTypes.OABFunction;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ABFunction;

	return oct_True;
}

oct_Bool oct_ABFunction_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OABFunction* out_result) {
	return oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_BFunction) * size + sizeof(oct_Uword), (void**)&out_result->ptr);
	// TODO: set all the function pointers to point to a built in function so that they have a value. Perhaps the identity function?
}

