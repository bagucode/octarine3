#include "oct_arraytype.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_ArrayType_initType(struct oct_Context* ctx) {
	oct_Type* at = ctx->rt->builtInTypes.ArrayType;
	oct_Bool result;
	at->variant = OCT_TYPE_STRUCT;
	at->structType.alignment = 0;
	at->structType.size = sizeof(oct_ArrayType);
	result = oct_OAField_alloc(ctx, 1, &at->structType.fields);
	if(!result) {
		return result;
	}
	at->structType.fields.ptr->data[0].offset = offsetof(oct_ArrayType, elementType);
	at->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}

oct_Bool _oct_FixedSizeArrayType_initType(struct oct_Context* ctx) {
	oct_Type* fat = ctx->rt->builtInTypes.FixedSizeArrayType;
	oct_Bool result;
	fat->variant = OCT_TYPE_STRUCT;
	fat->structType.alignment = 0;
	fat->structType.size = sizeof(oct_FixedSizeArrayType);
	result = oct_OAField_alloc(ctx, 2, &fat->structType.fields);
	if(!result) {
		return result;
	}
	fat->structType.fields.ptr->data[0].offset = offsetof(oct_FixedSizeArrayType, size);
	fat->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	fat->structType.fields.ptr->data[1].offset = offsetof(oct_FixedSizeArrayType, elementType);
	fat->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}
