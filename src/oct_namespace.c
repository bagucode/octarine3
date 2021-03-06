#include "oct_namespace.h"
#include "oct_runtime.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"
#include "oct_copyable.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_copyable.h"

#include <stddef.h>

typedef struct oct_Namespace {
	oct_String name;
	oct_Hashtable bindings;
	/* Mutex lock */
} oct_Namespace;

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Namespace_init(struct oct_Context* ctx) {

	// Namespace
	oct_CType t = ctx->rt->builtInTypes.Namespace;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Namespace);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Namespace, name);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.String;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Namespace, bindings);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Hashtable;

	// VTable for Object {dtor}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.NamespaceAsObject, t, oct_Namespace_dtor));

	// BNamespace
	t = ctx->rt->builtInTypes.BNamespace;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Namespace;

	// NamespaceOption
	t = ctx->rt->builtInTypes.NamespaceOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_NamespaceOption);
	CHECK(oct_ACType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.BNamespace;

	return oct_True;
}

#undef CHECK
#define CHECK(X) if(!X) goto error;

oct_Bool oct_Namespace_create(struct oct_Context* ctx, oct_OString nsName, oct_BNamespace* out_ns) {
	oct_NamespaceOption existing;
	oct_BString bstr;
	oct_BHashtable bTable;
	oct_HashtableKeyOption nameKey;
	oct_Any nsAny;
	oct_BNamespace newNs;
	oct_Bool result = oct_True;
	// TODO: lock namespace table
	bstr.ptr = nsName.ptr;
	CHECK(oct_Namespace_findNs(ctx, bstr, &existing));
	if(existing.variant == OCT_NAMESPACEOPTION_NAMESPACE) {
		*out_ns = existing.ns;
		CHECK(oct_String_destroyOwned(ctx, nsName));
		goto end;
	}
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Namespace), (void**)&newNs.ptr, "oct_Namespace_create"));
	bTable.ptr = &newNs.ptr->bindings;
	CHECK(oct_Hashtable_ctor(ctx, bTable, 100));

	// These two lines are a bit of a hack. The correct way would be to actually deep-copy the name string
	// into the NS name string and then use the owned name string as an owned hash key instead of a borrowed one
	newNs.ptr->name = (*nsName.ptr);
	OCT_FREEOWNED(nsName.ptr);

	CHECK(oct_Namespace_asObject(ctx, newNs, &nsAny.bobject));
	nsAny.variant = OCT_ANY_OOBJECT;
	bTable.ptr = &ctx->rt->namespaces;
	nameKey.variant = OCT_HASHTABLEKEYOPTION_BORROWED;
	bstr.ptr = &newNs.ptr->name;
	CHECK(oct_String_asHashtableKeyBorrowed(ctx, bstr, &nameKey.borrowed));
	CHECK(oct_Hashtable_put(ctx, bTable, nameKey, nsAny));

	out_ns->ptr = newNs.ptr;
	goto end;
error:
	result = oct_False;
end:
	// TODO: unlock namespace table
	return result;
}

oct_Bool oct_Namespace_findNs(struct oct_Context* ctx, oct_BString nsName, oct_NamespaceOption* out_ns) {
	oct_BHashtableKey nsNameKey;
	oct_Any ns;
	oct_BHashtable nsTable;
	oct_Bool result = oct_True;

	// TODO: lock namespace table (if not locked in create)
	
	CHECK(oct_String_asHashtableKeyBorrowed(ctx, nsName, &nsNameKey));
	nsTable.ptr = &ctx->rt->namespaces;
	CHECK(oct_Hashtable_borrow(ctx, nsTable, nsNameKey, &ns));
	if(ns.variant == OCT_ANY_NOTHING) {
		(*out_ns).variant = OCT_NAMESPACEOPTION_NOTHING;
	}
	else if(ns.bobject.vtable->type.ptr == ctx->rt->builtInTypes.BNamespace.ptr) {
		(*out_ns).variant = OCT_NAMESPACEOPTION_NAMESPACE;
		(*out_ns).ns.ptr = (oct_Namespace*)ns.bobject.self.self;
	}
	else {
		CHECK(oct_Context_setErrorWithCMessage(ctx, "Key found in namespace table but value was not a namespace"));
		goto error;
	}

	goto end;
error:
	result = oct_False;
end:
	// TODO: unlock namespace table (if not locked in create)
	return result;
}

oct_Bool oct_Namespace_bindInCurrent(struct oct_Context* ctx, oct_HashtableKeyOption key, oct_Any value) {
	oct_BNamespace bns;
	bns.ptr = ctx->ns;
	return oct_Namespace_bind(ctx, bns, key, value);
}

oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_HashtableKeyOption key, oct_Any value) {
	oct_BHashtable bindingsTable;
	oct_Bool result;
	bindingsTable.ptr = &ns.ptr->bindings;
	// TODO: check to make sure that if the value is not a type, function or other "permanent" object the value has to be owned!
	// TODO: lock bindings table
	result = oct_Hashtable_put(ctx, bindingsTable, key, value);
	// TODO: unlock bindings table
	return result;
}

oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_Any* out_value) {
	oct_BHashtable bindingsTable;
	oct_Bool result = oct_True;

	bindingsTable.ptr = &ns.ptr->bindings;
	// TODO: lock bindings table
	CHECK(oct_Hashtable_copyOrBorrow(ctx, bindingsTable, key, out_value));

	goto end;
error:
	result = oct_False;
end:
	// TODO: unlock bindings table
	return result;
}

oct_Bool oct_Namespace_asObject(struct oct_Context* ctx, oct_BNamespace ns, oct_BObject* out_obj) {
	out_obj->self.self = ns.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.NamespaceAsObject.ptr;
	return oct_True;
}

oct_Bool oct_Namespace_dtor(struct oct_Context* ctx, oct_BGeneric self) {
	oct_Namespace* ns = (oct_Namespace*)self.self;
	oct_Bool result;
	self.self = &ns->name;
	result = oct_String_dtor(ctx, self);
	self.self = &ns->bindings;
	return oct_Hashtable_dtor(ctx, self) && result;
}

