#include "oct_nothing.h"
#include "oct_nothing_functions.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Nothing_init(struct oct_Context* ctx) {

	// Nothing
	oct_BType t = ctx->rt->builtInTypes.Nothing;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Nothing);
	CHECK(oct_AField_createOwned(ctx, 0, &t.ptr->structType.fields));

	// Nothing VTable for Object {}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 0, &ctx->rt->vtables.NothingAsObject, t));

	// Nothing VTable for EqComparable {eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.EqComparable, 1, &ctx->rt->vtables.NothingAsEqComparable, t, oct_Nothing_equals));

	// Nothing VTable for Hashable {hash}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Hashable, 1, &ctx->rt->vtables.NothingAsHashable, t, oct_Nothing_hash));

	// Nothing VTable for HashtableKey {hash, eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.HashtableKey, 2, &ctx->rt->vtables.NothingAsHashtableKey, t, oct_Nothing_hash, oct_Nothing_equals));

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
	*out_hash = self.ptr->dummy * 983;
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
