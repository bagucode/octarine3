#include "oct_protocoltype.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Protocol_init(struct oct_Context* ctx) {
	oct_BType t;

	// Protocol
	t = ctx->rt->builtInTypes.Protocol;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_ProtocolType);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolType, functions);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OABFunction;

	// VTable
	// This is an incomplete type since it is not a proper array but still of unspecified size
	// It is defined here only to have a type for it to use when putting it in namespace bindings
	t = ctx->rt->builtInTypes.VTable;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_VTable);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_VTable, objectType);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.BType;

	// BVTable
	t = ctx->rt->builtInTypes.BVTable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.VTable;

	// ProtocolBinding
	t = ctx->rt->builtInTypes.ProtocolBinding;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_ProtocolBinding);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolBinding, protocolType);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.BType;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_ProtocolBinding, implementations);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Hashtable;

	// ProtocolBinding must participate in Object protocol
	// Bit of a chicken and egg problem here.
	// The init for the runtime must contain a hardcoded init for some of
	// the protocol bindings to make calls like this valid in the init phase
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 0, &ctx->rt->vtables.ProtocolBindingAsObject, t))

	// BProtocolBinding
	t = ctx->rt->builtInTypes.BProtocolBinding;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ProtocolBinding;

	return oct_True;
}

oct_Bool _oct_Protocol_addBuiltIn(struct oct_Context* ctx, oct_BProtocolBinding pb, oct_Uword fnCount, oct_BVTable* table, oct_BType type, ...) {
	va_list fns;
	oct_Uword i;
	CHECK(oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_BType) + (sizeof(void*) * fnCount), (void**)&table->ptr));
	va_start(fns, type);
	for(i = 0; i < fnCount; ++i) {
		table->ptr->functions[i] = va_arg(fns, void*);
	}
	va_end(fns);
	return oct_Protocol_addImplementation(ctx, pb, type, *table);
}

oct_Bool oct_Protocol_addImplementation(struct oct_Context* ctx, oct_BProtocolBinding protocol, oct_BType type, oct_BVTable vtable) {
	oct_BHashtable impls;
	// TODO: What to do with the fact that types and vtables are borrowed, not owned??
	// make a special hashtable that has borrowed keys and values to use for globals?
	oct_OHashtableKey key;
	oct_OObject val;
	oct_BSelf self;

	self.self = type.ptr;
	if(!oct_Object_as(ctx, self, ctx->rt->builtInTypes.Type, ctx->rt->builtInProtocols.HashtableKey, (oct_BObject*)&key)) {
		return oct_False;
	}
	self.self = vtable.ptr;
	if(!oct_Object_as(ctx, self, ctx->rt->builtInTypes.VTable, ctx->rt->builtInProtocols.Object, (oct_BObject*)&val)) {
		return oct_False;
	}

	impls.ptr = &protocol.ptr->implementations;
	return oct_Hashtable_put(ctx, impls, key, val);
}
