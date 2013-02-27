#include "oct_nothing.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_Nothing_init(struct oct_Context* ctx) {
	oct_BType t = ctx->rt->builtInTypes.Nothing;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Nothing);
	return oct_AField_createOwned(ctx, 0, &t.ptr->structType.fields);
}
