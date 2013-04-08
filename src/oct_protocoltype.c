#include "oct_protocoltype.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_exchangeheap.h"
#include "oct_any.h"

#include <stddef.h>
#include <stdarg.h>

#define CHECK(X) if(!X) return oct_False;

static oct_Bool oct_Bool_VTableDtor(struct oct_Context* ctx, oct_BSelf vtable) {
	oct_VTable* vt = (oct_VTable*)vtable.self;
	return OCT_FREEOWNED(vt->functions);
}

oct_Bool _oct_VTable_init(struct oct_Context* ctx) {
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ObjectVTable), (void**)&ctx->rt->vtables.VTableAsObject.ptr, "_oct_VTable_init"));
	ctx->rt->vtables.VTableAsObject.ptr->objectType = ctx->rt->builtInTypes.VTable;
	ctx->rt->vtables.VTableAsObject.ptr->functions[0] = oct_Bool_VTableDtor;
	return oct_True;
}

oct_Bool _oct_Protocol_init(struct oct_Context* ctx) {
	oct_CType t;

	// Protocol
	t = ctx->rt->builtInTypes.Protocol;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_ProtocolType);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolType, functions);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OACFunction;

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

	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.ProtocolBindingAsObject, t, oct_ProtocolBinding_dtor));

	// BProtocolBinding
	t = ctx->rt->builtInTypes.BProtocolBinding;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ProtocolBinding;

	return oct_True;
}

oct_Bool oct_ProtocolBinding_dtor(struct oct_Context* ctx, oct_BSelf self) {
	oct_ProtocolBinding* pb = (oct_ProtocolBinding*)self.self;
	self.self = &pb->implementations;
	return oct_Hashtable_dtor(ctx, self);
}

oct_Bool _oct_Protocol_addBuiltIn(struct oct_Context* ctx, oct_BProtocolBinding pb, oct_Uword fnCount, oct_CVTable* table, oct_CType type, ...) {
	va_list fns;
	oct_Uword i;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * fnCount), (void**)&table->ptr, "_oct_Protocol_addBuiltIn"));
	table->ptr->objectType = type;
	va_start(fns, type);
	for(i = 0; i < fnCount; ++i) {
		table->ptr->functions[i] = va_arg(fns, void*);
	}
	va_end(fns);
	return oct_Protocol_addImplementation(ctx, pb, type, *table);
}

static oct_Bool oct_VTable_asObject(oct_Context* ctx, oct_CVTable vtable, oct_BObject* out_obj) {
	out_obj->self.self = vtable.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.VTableAsObject.ptr;
	return oct_True;
}

oct_Bool oct_Protocol_addImplementation(struct oct_Context* ctx, oct_BProtocolBinding protocol, oct_CType type, oct_CVTable vtable) {
	oct_BHashtable impls;
	oct_HashtableKeyOption key;
	oct_Any val;
	key.variant = OCT_HASHTABLEKEYOPTION_BORROWED;
	CHECK(oct_CType_asHashtableKey(ctx, type, &key.borrowed));
	CHECK(oct_VTable_asObject(ctx, vtable, &val.bobject));
	val.variant = OCT_ANY_BOBJECT;
	impls.ptr = &protocol.ptr->implementations;
	return oct_Hashtable_put(ctx, impls, key, val);
}

