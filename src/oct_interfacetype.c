#include "oct_interfaceType.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"

#include <stddef.h>

oct_Bool _oct_Interface_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.InterfaceType;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.size = sizeof(oct_InterfaceType);
	t->structType.alignment = 0;
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_InterfaceType, functions);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OABFunction;
	return oct_True;
}

