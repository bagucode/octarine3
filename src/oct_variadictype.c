#include "oct_variadictype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

oct_Bool _oct_VariadicType_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.VariadicType;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_VariadicType);
    if(!oct_OAField_alloc(ctx, 3, &t->structType.fields)) {
        return oct_False;
    }
	t->structType.fields.ptr->data[0].offset = offsetof(oct_VariadicType, alignment);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_VariadicType, size);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[2].offset = offsetof(oct_VariadicType, types);
	t->structType.fields.ptr->data[2].type.ptr = ctx->rt->builtInTypes.OABType;
	return oct_True;
}

