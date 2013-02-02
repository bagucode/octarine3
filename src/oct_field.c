#include "oct_field.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

// Private

oct_Bool _oct_Field_initType(struct oct_Context* ctx) {
	oct_Type* ft = ctx->rt->builtInTypes.Field;
	oct_Bool result;
	ft->variant = OCT_TYPE_STRUCT;
	ft->structType.alignment = 0;
	ft->structType.size = sizeof(oct_Field);
	result = oct_OAField_alloc(ctx, 2, &ft->structType.fields);
	if(!result) {
		return result;
	}
	ft->structType.fields.ptr->data[0].offset = offsetof(oct_Field, offset);
	ft->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	ft->structType.fields.ptr->data[1].offset = offsetof(oct_Field, type);
	ft->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}

oct_Bool _oct_AField_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.AField->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AField->arrayType.elementType.ptr = ctx->rt->builtInTypes.Field;
	return oct_True;
}

oct_Bool _oct_OAField_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OAField->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAField->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAField->pointerType.type.ptr = ctx->rt->builtInTypes.AField;
	return oct_True;
}

oct_Bool oct_Field_ctor(struct oct_Context* ctx, oct_Field* field) {
	// BType cannot be null so we initialize it to point to Nothing
	field->offset = 0;
	field->type.ptr = ctx->rt->builtInTypes.Nothing;
	return oct_True;
}

oct_Bool oct_Field_dtor(struct oct_Context* ctx, oct_Field* field) {
	// noop, field does not point to anything owned or shared
	return oct_True;
}

oct_Bool oct_OAField_alloc(struct oct_Context* ctx, oct_Uword size, oct_OAField* out_result) {
	oct_Uword i;
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_AField) + (sizeof(oct_Field) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }

	out_result->ptr->size = size;
	for(i = 0; i < size; ++i) {
		oct_Field_ctor(ctx, &out_result->ptr->data[i]);
	}
	return oct_True;
}

oct_Bool oct_OAField_free(struct oct_Context* ctx, oct_OAField oafield) {
	// No need to call destructor since it does nothing
    oct_ExchangeHeap_free(ctx, oafield.ptr);
	return oct_True;
}
