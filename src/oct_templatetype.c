#include "oct_templatetype.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stdlib.h>
#include <stddef.h>

// Private

oct_Bool _oct_TemplateType_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.TemplateType;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_TemplateType);
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_TemplateType, params);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OATemplateParam;
	return oct_True;
}

oct_Bool _oct_TemplateParam_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.TemplateParam;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_TemplateParam);
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_TemplateParam, variant);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_TemplateParam, type);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}

oct_Bool _oct_ATemplateParam_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.ATemplateParam->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ATemplateParam->arrayType.elementType.ptr = ctx->rt->builtInTypes.TemplateParam;
	return oct_True;
}

// Public

oct_Bool oct_TemplateParam_ctor(struct oct_Context* ctx, oct_TemplateParam* tparam) {
	tparam->type.ptr = ctx->rt->builtInTypes.Type;
	tparam->variant = OCT_TEMPLATEPARAM_TEMPLATE;
	return oct_True;
}

oct_Bool oct_TemplateParam_dtor(struct oct_Context* ctx, oct_TemplateParam* tparam) {
	// noop, does not own anything
	return oct_True;
}

oct_Bool oct_OATemplateParam_alloc(struct oct_Context* ctx, oct_Uword size, oct_OATemplateParam* out_result) {
	oct_Uword i;
	out_result->ptr = (oct_ATemplateParam*)malloc(sizeof(oct_ATemplateParam) + (sizeof(oct_TemplateParam) * size));
	if(out_result->ptr == NULL) {
		// TODO: set ctx error to OOM
		return oct_False;
	}
	out_result->ptr->size = size;
	for(i = 0; i < size; ++i) {
		oct_TemplateParam_ctor(ctx, &out_result->ptr->data[i]);
	}
	return oct_True;
}

oct_Bool oct_OATemplateParam_free(struct oct_Context* ctx, oct_OATemplateParam oatparam) {
	// destructor is noop, no need to call
	free(oatparam.ptr);
	return oct_True;
}

