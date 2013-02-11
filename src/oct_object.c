#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"

oct_Bool _oct_Object_initType(struct oct_Context* ctx) {
}

oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_Object obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.Symbol == obj.vtable->instanceType.ptr;
	return oct_True;
}

oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_Object obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.String == obj.vtable->instanceType.ptr;
	return oct_True;
}

oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_Object obj, oct_Bool* out_bool) {
	*out_bool = ctx->rt->builtInTypes.List == obj.vtable->instanceType.ptr;
	return oct_True;
}
