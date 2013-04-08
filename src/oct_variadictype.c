#include "oct_variadictype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_VariadicType_init(struct oct_Context* ctx) {
	oct_CType t = ctx->rt->builtInTypes.Variadic;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_VariadicType);
    CHECK(oct_AField_createOwned(ctx, 3, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_VariadicType, alignment);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_VariadicType, size);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[2].offset = offsetof(oct_VariadicType, types);
	t.ptr->structType.fields.ptr->data[2].type = ctx->rt->builtInTypes.OABType;
	return oct_True;
}

