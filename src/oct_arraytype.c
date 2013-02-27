#include "oct_arraytype.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Array_init(struct oct_Context* ctx) {
	oct_BType t = ctx->rt->builtInTypes.Array;

	// Array
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_ArrayType);
	CHECK(oct_OAField_alloc(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ArrayType, elementType);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.BType;

	// Fixed
	t = ctx->rt->builtInTypes.FixedSizeArray;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_FixedSizeArrayType);
	CHECK(oct_OAField_alloc(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_FixedSizeArrayType, size);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_FixedSizeArrayType, elementType);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.BType;

	return oct_True;
}
