#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"
#include "oct_exchangeheap.h"
#include "oct_type.h"
#include "oct_function.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Object_init(struct oct_Context* ctx) {

	// Object protocol. No functions.
	oct_BType t = ctx->rt->builtInTypes.Object;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 0, &t.ptr->protocolType.functions));

	// OObject
	t = ctx->rt->builtInTypes.OObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// BObject
	t = ctx->rt->builtInTypes.BObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// OObjectOption
	t = ctx->rt->builtInTypes.OObjectOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OObjectOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OObject;

	return oct_True;
}

// The output is BObject because C does not have templates but the output should be safe to manually cast to the given protocol
oct_Bool oct_Object_as(oct_Context* ctx, oct_BSelf object, oct_BType selfType, oct_BProtocolBinding protocol, oct_BObject* out_casted) {
	oct_BHashtable table;
	oct_BHashtableKey key;
	oct_BObject vtableObject;

	table.ptr = &protocol.ptr->implementations;
	CHECK(oct_BType_asHashtableKey(ctx, selfType, &key));
	// TODO: change hashtable to return OObjectOption/BObjectOption instead of just OObject/BObject
	CHECK(oct_Hashtable_borrow(ctx, table, key, &vtableObject));
	out_casted->self = object;
	out_casted->vtable = (oct_ObjectVTable*)vtableObject.self.self;
	return oct_True;
}

oct_Bool oct_Object_postWalk(oct_Context* ctx, oct_OSelf root, oct_BType rootType, oct_PostwalkFn fn) {

}

static oct_Bool destroyObject(oct_Context* ctx, oct_OSelf obj, oct_BType type, oct_OSelf* out) {
	CHECK(oct_ExchangeHeap_free(ctx, obj.self, type));
	out->self = NULL;
	return oct_True;
}

oct_Bool oct_Object_destroyOwned(oct_Context* ctx, oct_OObject obj) {
	return oct_Object_postWalk(ctx, obj.self, obj.vtable->type, destroyObject);
}

