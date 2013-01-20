#include "oct_option.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

oct_Bool _oct_OptionT_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.OptionT;
	oct_Bool result;
	t->variant = OCT_TYPE_TEMPLATE;
	t->templateType.targetType = OCT_TEMPLATETARGET_VARIADIC;
	result = oct_OATemplateParam_alloc(ctx, 2, &t->templateType.params);
	if(!result) {
		return result;
	}
	t->templateType.params.ptr->data[0].variant = OCT_TEMPLATEPARAM_NORMAL;
	t->templateType.params.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Nothing;
	t->templateType.params.ptr->data[1].variant = OCT_TEMPLATEPARAM_TEMPLATE;
	t->templateType.params.ptr->data[1].type.ptr = ctx->rt->builtInTypes.Type;
	return oct_True;
}
