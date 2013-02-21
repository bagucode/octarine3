#include "oct_protocoltype.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"

#include <stddef.h>

static oct_VTable ProtocolBindingObjectVTable;
static oct_BVTable BProtocolBindingObjectVTable;

oct_Bool _oct_Protocol_init(struct oct_Context* ctx) {
	oct_BType t;
	oct_Bool result;

	// Protocol
	t = ctx->rt->builtInTypes.Protocol;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_ProtocolType);
	t.ptr->structType.alignment = 0;
	result = oct_OAField_alloc(ctx, 1, &t.ptr->structType.fields);
	if(!result) {
		return result;
	}
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolType, functions);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OABFunction;

	// VTable
	// This is an incomplete type since it is not a proper array but still of unspecified size
	// It is defined here only to have a type for it to use when putting it in namespace bindings
	t = ctx->rt->builtInTypes.VTable;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_VTable);
	t.ptr->structType.alignment = 0;
	result = oct_OAField_alloc(ctx, 1, &t.ptr->structType.fields);
	if(!result) {
		return result;
	}
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
	result = oct_OAField_alloc(ctx, 2, &t.ptr->structType.fields);
	if(!result) {
		return result;
	}
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolBinding, protocolType);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.BType;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_ProtocolBinding, implementations);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Hashtable;

	// ProtocolBinding must participate in Object protocol
	// Bit of a chicken and egg problem here.
	// The init for the runtime must contain a hardcoded init for some of
	// the protocol bindings to make calls like this valid in the init phase
	ProtocolBindingObjectVTable.objectType = t;
	BProtocolBindingObjectVTable.ptr = &ProtocolBindingObjectVTable;
	result = oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Object, t, BProtocolBindingObjectVTable);
	if(!result) {
		return result;
	}

	// BProtocolBinding
	t = ctx->rt->builtInTypes.BProtocolBinding;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ProtocolBinding;

	return oct_True;
}

oct_Bool oct_Protocol_addImplementation(struct oct_Context* ctx, oct_BProtocolBinding protocol, oct_BType type, oct_BVTable vtable) {
	oct_BHashtable impls;
	// TODO: What to do with the fact that types and vtables are borrowed, not owned??
	// make a special hashtable that has borrowed keys and values to use for globals?
	oct_OHashtableKey key;
	oct_OObject val;
	oct_BSelf self;

	self.self = type.ptr;
	if(!oct_Object_as(ctx, self, ctx->rt->builtInTypes.Type, ctx->rt->builtInTypes.HashtableKey, (oct_BObject*)&key)) {
		return oct_False;
	}
	self.self = vtable.ptr;
	if(!oct_Object_as(ctx, self, ctx->rt->builtInTypes.VTable, ctx->rt->builtInTypes.Object, (oct_BObject*)&val)) {
		return oct_False;
	}

	impls.ptr = &protocol.ptr->implementations;
	return oct_Hashtable_put(ctx, impls, key, val);
}
