#include "oct_primitives_functions.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_protocoltype.h"

#include <stdio.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Primitives_initProtocols(struct oct_Context* ctx) {
    CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Printable, 1, &ctx->rt->vtables.I32AsPrintable, ctx->rt->builtInTypes.I32, oct_I32_print));
    CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Printable, 1, &ctx->rt->vtables.F32AsPrintable, ctx->rt->builtInTypes.F32, oct_F32_print));
    
    return oct_True;
}

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

oct_Bool oct_I32_print(struct oct_Context* ctx, oct_BI32 i32) {
    printf("%d", *i32.ptr);
    return oct_True;
}

oct_Bool oct_F32_print(struct oct_Context* ctx, oct_BF32 f32) {
    printf("%f", *f32.ptr);
    return oct_True;
}
