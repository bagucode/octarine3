#include "oct_structtype.h"
#include "oct_type.h"
#include "oct_runtime.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_StructType_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.StructType;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_StructType);
	result = oct_OAField_alloc(ctx, 3, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_StructType, alignment);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_StructType, size);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[2].offset = offsetof(oct_StructType, fields);
	t->structType.fields.ptr->data[2].type.ptr = ctx->rt->builtInTypes.OAField;
	return oct_True;
}
