#include "oct_structtype.h"
#include "oct_type.h"
#include "oct_runtime.h"
#include "oct_context.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_StructType_init(struct oct_Context* ctx) {
	oct_CType t = ctx->rt->builtInTypes.Struct;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_StructType);
	CHECK(oct_AField_createOwned(ctx, 3, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_StructType, alignment);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_StructType, size);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[2].offset = offsetof(oct_StructType, fields);
	t.ptr->structType.fields.ptr->data[2].type = ctx->rt->builtInTypes.OAField;
	return oct_True;
}
