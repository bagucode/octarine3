#include "oct_pointertype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_PointerType_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.PointerType;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_PointerType);
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_PointerType, kind);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_PointerType, type);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}
