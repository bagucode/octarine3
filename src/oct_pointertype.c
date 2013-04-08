#include "oct_pointertype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_PointerType_init(struct oct_Context* ctx) {
	oct_CType t = ctx->rt->builtInTypes.Pointer;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_PointerType);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_PointerType, kind);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_PointerType, type);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.BType;
	return oct_True;
}
