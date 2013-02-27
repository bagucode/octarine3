#include "oct_prototype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_ProtoType_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ProtoType;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_ProtoType);
	result = oct_AField_createOwned(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_ProtoType, dummy);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	return oct_True;
}
