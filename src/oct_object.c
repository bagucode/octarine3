#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"

oct_Bool _oct_Object_initType(struct oct_Context* ctx) {
}

oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.Symbol == obj.object.vtable->instanceType.ptr;
	return oct_True;
}

oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.String == obj.object.vtable->instanceType.ptr;
	return oct_True;
}

oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.List == obj.object.vtable->instanceType.ptr;
	return oct_True;
}
