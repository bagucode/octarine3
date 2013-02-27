#include "oct_field.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

// Private

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Field_init(struct oct_Context* ctx) {

	// Field
	oct_BType t = ctx->rt->builtInTypes.Field;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Field);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Field, offset);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Field, type);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.BType;

	// AField
	ctx->rt->builtInTypes.AField.ptr->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AField.ptr->arrayType.elementType = ctx->rt->builtInTypes.Field;

	// OAField
	ctx->rt->builtInTypes.OAField.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAField.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAField.ptr->pointerType.type = ctx->rt->builtInTypes.AField;

	return oct_True;
}

oct_Bool oct_AField_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAField* out_result) {
	oct_Uword i;
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_AField) + (sizeof(oct_Field) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }

	out_result->ptr->size = size;
	for(i = 0; i < size; ++i) {
		out_result->ptr->data[i].offset = 0;
		out_result->ptr->data[i].type = ctx->rt->builtInTypes.Nothing;
	}
	return oct_True;
}

oct_Bool oct_AField_destroyOwned(struct oct_Context* ctx, oct_OAField oafield) {
    oct_ExchangeHeap_freeRaw(ctx, oafield.ptr);
	return oct_True;
}
