#include "oct_nothing.h"
#include "oct_nothing_functions.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Nothing_VTableInit(struct oct_Context* ctx) {
	// Object
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.NothingAsObject.ptr, "_oct_Nothing_VTableInit, Object"));
	ctx->rt->vtables.NothingAsObject.ptr->functions[0] = oct_Nothing_dtor;
	ctx->rt->vtables.NothingAsObject.ptr->objectType = ctx->rt->builtInTypes.Nothing;

	// EqComparable
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.NothingAsEqComparable.ptr, "_oct_Nothing_VTableInit, EqComparable"));
	ctx->rt->vtables.NothingAsEqComparable.ptr->functions[0] = oct_Nothing_equals;
	ctx->rt->vtables.NothingAsEqComparable.ptr->objectType = ctx->rt->builtInTypes.Nothing;
	
	// Hashable
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.NothingAsHashable.ptr, "_oct_Nothing_VTableInit, Hashable"));
	ctx->rt->vtables.NothingAsHashable.ptr->functions[0] = oct_Nothing_hash;
	ctx->rt->vtables.NothingAsHashable.ptr->objectType = ctx->rt->builtInTypes.Nothing;
	
	// HashtableKey (already allocated in runtime init)
	ctx->rt->vtables.NothingAsHashtableKey.ptr->functions[0] = oct_Nothing_hash;
	ctx->rt->vtables.NothingAsHashtableKey.ptr->functions[1] = oct_Nothing_equals;

	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Object, ctx->rt->builtInTypes.Nothing, ctx->rt->vtables.NothingAsObject);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.EqComparable, ctx->rt->builtInTypes.Nothing, ctx->rt->vtables.NothingAsEqComparable);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Hashable, ctx->rt->builtInTypes.Nothing, ctx->rt->vtables.NothingAsHashable);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.HashtableKey, ctx->rt->builtInTypes.Nothing, ctx->rt->vtables.NothingAsHashtableKey);
    
    return oct_True;
}

oct_Bool _oct_Nothing_init(struct oct_Context* ctx) {

	// Nothing
	oct_CType t = ctx->rt->builtInTypes.Nothing;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Nothing);
	CHECK(oct_AField_createOwned(ctx, 0, &t.ptr->structType.fields));

	// BNothing
	t = ctx->rt->builtInTypes.BNothing;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Nothing;

	return oct_True;
}

oct_Bool oct_Nothing_equals(struct oct_Context* ctx, oct_BNothing self, oct_BNothing other, oct_Bool* out_eq) {
	*out_eq = self.ptr->dummy == other.ptr->dummy; // Change this?
	return oct_True;
}

oct_Bool oct_Nothing_hash(struct oct_Context* ctx, oct_BNothing self, oct_Uword* out_hash) {
	*out_hash = ((oct_Uword)self.ptr) * 983;
	return oct_True;
}

oct_Bool oct_Nothing_asObject(struct oct_Context* ctx, oct_BNothing nothing, oct_BObject* out_obj) {
	out_obj->self.self = nothing.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.NothingAsObject.ptr;
	return oct_True;
}

oct_Bool oct_Nothing_asHashtableKey(struct oct_Context* ctx, oct_BNothing nothing, oct_BHashtableKey* out_key) {
	out_key->self.self = nothing.ptr;
	out_key->vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.NothingAsHashtableKey.ptr;
	return oct_True;
}

oct_Bool oct_Nothing_dtor(struct oct_Context* ctx, oct_BGeneric self) {
	return oct_True;
}
