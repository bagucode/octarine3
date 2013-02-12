#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"
#include "oct_exchangeheap.h"

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

oct_Bool oct_Object_ctor(struct oct_Context* ctx, oct_Object obj, oct_OList args) {
	return obj.vtable->ctor(ctx, obj.object, args);
}

oct_Bool oct_Object_dtor(struct oct_Context* ctx, oct_Object obj) {
	return obj.vtable->dtor(ctx, obj.object);
}

//oct_Bool oct_Object_print(struct oct_Context* ctx, oct_Object obj, /*Text output stream*/);

oct_Bool oct_Object_invoke(struct oct_Context* ctx, oct_Object obj, oct_OList args) {
	return obj.vtable->invoke(ctx, obj.object, args);
}

//oct_Bool oct_Object_eval(struct oct_Context* ctx, oct_Object obj);

oct_Bool oct_Object_copyOwned(struct oct_Context* ctx, oct_Object obj, void** out_copy) {
	return obj.vtable->copyOwned(ctx, obj.object, out_copy);
}

oct_Bool oct_Object_copyManaged(struct oct_Context* ctx, oct_Object obj, void** out_copy) {
	return obj.vtable->copyManaged(ctx, obj.object, out_copy);
}

oct_Bool oct_Object_hash(struct oct_Context* ctx, oct_Object obj, oct_Uword* out_hash) {
	return obj.vtable->hash(ctx, obj.object, out_hash);
}

oct_Bool oct_Object_equals(struct oct_Context* ctx, oct_Object obj, oct_BObject other, oct_Bool* out_result) {
	return obj.vtable->equals(ctx, obj.object, other, out_result);
}

oct_Bool oct_Object_destroyOwned(struct oct_Context* ctx, oct_OObject obj) {
	oct_Bool result = oct_Object_dtor(ctx, obj.object);
	return oct_ExchangeHeap_free(ctx, obj.object.object) && result;
}
