#include "oct_primitives_functions.h"
#include "oct_context.h"
#include "oct_runtime.h"

oct_Bool oct_I32_asObject(struct oct_Context* ctx, oct_OI32 i32, oct_OObject* out_obj) {
	out_obj->self.self = i32.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.I32AsObject.ptr;
	return oct_True;
}

oct_Bool oct_F32_asObject(struct oct_Context* ctx, oct_OF32 f32, oct_OObject* out_obj) {
	out_obj->self.self = f32.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.F32AsObject.ptr;
	return oct_True;
}
