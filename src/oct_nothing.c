#include "oct_nothing.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_Nothing_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Nothing;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_Nothing);
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_Nothing, dummy);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	return oct_True;
}
