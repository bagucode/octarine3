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
	return oct_AField_createOwned(ctx, 0, &t->structType.fields);
}
