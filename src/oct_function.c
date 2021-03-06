#include "oct_function.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Function_init(struct oct_Context* ctx) {

	// Function
	oct_CType t = ctx->rt->builtInTypes.Function;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_Function);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Function, paramTypes);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OABType;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Function, returnTypes);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OABType;

	// CFunction
	t = ctx->rt->builtInTypes.CFunction;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Function;

	// ACFunction
	t = ctx->rt->builtInTypes.ACFunction;
	t.ptr->variant = OCT_TYPE_ARRAY;
	t.ptr->arrayType.elementType = ctx->rt->builtInTypes.CFunction;

	// OACFunction
	t = ctx->rt->builtInTypes.OACFunction;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ACFunction;

	return oct_True;
}

oct_Bool oct_ACFunction_createOwned(struct oct_Context* ctx, oct_Uword size, oct_CACFunction* out_result) {
	oct_Bool result = OCT_ALLOCOWNED(sizeof(oct_CFunction) * size + sizeof(oct_Uword), (void**)&out_result->ptr, "oct_ACFunction_createOwned");
	// TODO: set all the function pointers to point to a built in function so that they have a value. Perhaps the identity function?
	out_result->ptr->size = size;
	return result;
}

oct_Bool oct_Function_dtor(struct oct_Context* ctx, oct_BGeneric self) {
	oct_CFunction fn;
	oct_Bool result;
	fn.ptr = (oct_Function*)self.self;
	result = OCT_FREEOWNED(fn.ptr->paramTypes.ptr);
	return OCT_FREEOWNED(fn.ptr->returnTypes.ptr) && result;
}
