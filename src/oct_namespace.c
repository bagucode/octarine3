#include "oct_namespace.h"
#include "oct_runtime.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"
#include "oct_copyable.h"
#include "oct_type.h"
#include "oct_type_pointers.h"

#include <stddef.h>

typedef struct oct_Namespace {
	oct_String name;
	oct_Hashtable bindings;
	/* Mutex lock */
} oct_Namespace;

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Namespace_init(struct oct_Context* ctx) {

	// Namespace
	oct_BType t = ctx->rt->builtInTypes.Namespace;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Namespace);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Namespace, name);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.String;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Namespace, bindings);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Hashtable;

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
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
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
	oct_OHashtableKey nameKey;
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

	CHECK(oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_Namespace), (void**)&newNs.ptr));
	bTable.ptr = &newNs.ptr->bindings;
	CHECK(oct_Hashtable_ctor(ctx, bTable, 100));
	newNs.ptr->name = (*nsName.ptr);

	CHECK(oct_Namespace_asObject(ctx, newNs, &nsAny.bobject));
	nsAny.variant = OCT_ANY_BOBJECT;

	bTable.ptr = &ctx->rt->namespaces;
	CHECK(oct_String_asHashtableKeyOwned(ctx, nsName, &nameKey));
	CHECK(oct_Hashtable_put(ctx, bTable, nameKey, nsAny));

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
	return oct_True;
}

oct_Bool oct_Namespace_bindInCurrent(struct oct_Context* ctx, oct_OHashtableKey key, oct_Any value) {
	oct_BNamespace bns;
	bns.ptr = ctx->ns;
	return oct_Namespace_bind(ctx, bns, binding);
}

oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_OHashtableKey key, oct_Any value) {
	oct_BHashtable bindingsTable;
	oct_Bool result = oct_True;

	bindingsTable.ptr = &ns.ptr->bindings;
	CHECK(oct_Hashtable_put(ctx, bindingsTable, binding.key, binding.object));

	goto end;
error:
	result = oct_False;
end:
	return result;
}

oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BHashtableKey key, oct_Any* out_value) {
}

oct_Bool oct_Namespace_cBind(struct oct_Context* ctx, const char* keySym, oct_OObject obj) {
	oct_OSymbol sym;
	oct_BSymbol bs;
	oct_OString str;
	oct_BHashtableKey bhk;
	oct_BindingInfo bi;
	oct_Bool result = oct_True;

	CHECK(oct_String_createOwnedFromCString(ctx, keySym, &str));
	CHECK(oct_Symbol_createOwned(ctx, str, &sym));
	bs.ptr = sym.ptr;
	CHECK(oct_Symbol_asHashtableKey(ctx, bs, &bhk));
	bi.object = obj;
	bi.key.self.self = bhk.self.self;
	bi.key.vtable = bhk.vtable;
	CHECK(oct_Namespace_bindInCurrent(ctx, bi));

	goto end;
error:
	result = oct_False;
end:
	return result;
}

// ********************************************************************************************************

oct_Bool oct_Namespace_copyValueOwned(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_OObject* out_obj) {
	oct_BHashtable bindingsTable;
	oct_BObject borrowed;
	oct_BCopyable cp;
	oct_Bool result = oct_True;

	bindingsTable.ptr = &ns.ptr->bindings;
	CHECK(oct_Hashtable_borrow(ctx, bindingsTable, key, &borrowed));
	// cast
	CHECK(oct_Object_cast(ctx, borrowed, _oct_BCopyableType, &borrowed));
	cp.self.self = borrowed.self.self;
	cp.vtable = (oct_CopyableVTable*)borrowed.vtable;
	// copy
	CHECK(oct_Copyable_copyOwned(ctx, cp, out_obj));
	goto end;
error:
	result = oct_False;
end:
	return result;
}

oct_Bool oct_Namespace_borrowGlobal(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_BObject* out_obj) {
	oct_BHashtable bindingsTable;
	oct_Bool result = oct_True;

	bindingsTable.ptr = &ns.ptr->bindings;
	// TODO: check that the binding is not a value!
	// only functions, types and protocols are allowed to be globally borrowed, values must be copied
	CHECK(oct_Hashtable_borrow(ctx, bindingsTable, key, out_obj));
	goto end;
error:
	result = oct_False;
end:
	return result;
}



//#include "oct_runtime.h"
//#include "oct_type.h"
//#include "oct_context.h"
//#include "oct_string.h"
//#include "oct_error.h"
//
//#include <stddef.h>
//#include <stdlib.h>
//
//#define CHECK(X) if(!X) return oct_False;
//
//oct_Bool _oct_NamespaceBinding_initType(struct oct_Context* ctx) {
//	oct_Type* t = ctx->rt->builtInTypes.NamespaceBinding;
//
//	t->variant = OCT_TYPE_STRUCT;
//	t->structType.alignment = 0;
//	t->structType.size = sizeof(oct_NamespaceBinding);
//	CHECK(oct_AField_createOwned(ctx, 2, &t->structType.fields));
//	t->structType.fields.ptr->data[0].offset = offsetof(oct_NamespaceBinding, sym);
//	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Symbol;
//	t->structType.fields.ptr->data[1].offset = offsetof(oct_NamespaceBinding, obj);
//	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.OObjectOption;
//	return oct_True;
//}
//
//oct_Bool _oct_ANamespaceBinding_initType(struct oct_Context* ctx) {
//	ctx->rt->builtInTypes.ANamespaceBinding->variant = OCT_TYPE_ARRAY;
//	ctx->rt->builtInTypes.ANamespaceBinding->arrayType.elementType.ptr = ctx->rt->builtInTypes.NamespaceBinding;
//	return oct_True;
//}
//
//oct_Bool _oct_OANamespaceBinding_initType(struct oct_Context* ctx) {
//	ctx->rt->builtInTypes.OANamespaceBinding->variant = OCT_TYPE_POINTER;
//	ctx->rt->builtInTypes.OANamespaceBinding->pointerType.kind = OCT_POINTER_OWNED;
//	ctx->rt->builtInTypes.OANamespaceBinding->pointerType.type.ptr = ctx->rt->builtInTypes.ANamespaceBinding;
//	return oct_True;
//}
//
//oct_Bool _oct_Namespace_initType(struct oct_Context* ctx) {
//	oct_Type* t = ctx->rt->builtInTypes.Namespace;
//
//	t->variant = OCT_TYPE_STRUCT;
//	t->structType.alignment = 0;
//	t->structType.size = sizeof(oct_Namespace);
//	CHECK(oct_AField_createOwned(ctx, 1, &t->structType.fields));
//	t->structType.fields.ptr->data[0].offset = offsetof(oct_Namespace, bindings);
//	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OANamespaceBinding;
//	return oct_True;
//}
//
//oct_Bool _oct_BNamespace_initType(struct oct_Context* ctx) {
//	ctx->rt->builtInTypes.BNamespace->variant = OCT_TYPE_POINTER;
//	ctx->rt->builtInTypes.BNamespace->pointerType.kind = OCT_POINTER_BORROWED;
//	ctx->rt->builtInTypes.BNamespace->pointerType.type.ptr = ctx->rt->builtInTypes.Namespace;
//	return oct_True;
//}
//
//oct_Bool oct_Namespace_create(struct oct_Context* ctx, oct_OSymbol name, oct_BNamespace* out_ns) {
//	oct_NamespaceList *newNode, *lastNode;
//	oct_Uword i;
//
//	// TODO: Lock namespace collection
//	// TODO: just return existing namespace if there is a name clash
//	newNode = (oct_NamespaceList*)malloc(sizeof(oct_NamespaceList));
//	if(!newNode) {
//		oct_Context_setErrorOOM(ctx);
//		// TODO: unlock namespace collection
//		return oct_False;
//	}
//	newNode->name = name;
//	newNode->next = NULL;
//
//	// create bindings array, start with 10
//	newNode->ns.bindings.ptr = (oct_ANamespaceBinding*)malloc(sizeof(oct_ANamespaceBinding) + (sizeof(oct_NamespaceBinding) * 10));
//	if(!newNode->ns.bindings.ptr) {
//		free(newNode);
//		oct_Context_setErrorOOM(ctx);
//		// TODO: unlock namespace collection
//		return oct_False;
//	}
//	newNode->ns.bindings.ptr->size = 10;
//
//	// init bindings to nothing
//	for(i = 0; i < 10; ++i) {
//		newNode->ns.bindings.ptr->bindings[i].sym.variant = OCT_SYMBOLOPTION_NOTHING;
//		newNode->ns.bindings.ptr->bindings[i].sym.nothing.dummy = 0;
//		newNode->ns.bindings.ptr->bindings[i].obj.variant = OCT_OBJECTOPTION_NOTHING;
//		newNode->ns.bindings.ptr->bindings[i].obj.nothing.dummy = 0;
//	}
//
//	// put new node in list
//	if(!ctx->rt->namespaces) {
//		ctx->rt->namespaces = newNode;
//	}
//	else {
//		lastNode = ctx->rt->namespaces;
//		while(lastNode->next) {
//			lastNode = lastNode->next;
//		}
//		lastNode->next = newNode;
//	}
//	// TODO: unlock namespace collection
//	out_ns->ptr = &newNode->ns;
//	return oct_True;
//}
//
//oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_OSymbol sym, oct_OObjectOption val) {
//	oct_Uword i, j;
//	oct_Uword newSize;
//    oct_Uword ptrKind;
//	oct_OANamespaceBinding newBindings;
//    
//	// TODO: lock bindings
//	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
//		if(ns.ptr->bindings.ptr->bindings[i].sym.variant == OCT_SYMBOLOPTION_NOTHING) {
//			// Free slot
//			// TODO: save free slot index but check all because we can't have duplicates!
//			goto ok;
//		}
//	}
//	// No free bindings. Expand array.
//	newSize = ns.ptr->bindings.ptr->size * 2;
//	newBindings.ptr = (oct_ANamespaceBinding*)malloc(sizeof(oct_ANamespaceBinding) + (sizeof(oct_NamespaceBinding) * newSize));
//	if(newBindings.ptr == NULL) {
//		oct_Context_setErrorOOM(ctx);
//		// TODO: unlock bindings
//		return oct_False; // OOM
//	}
//	newBindings.ptr->size = newSize;
//	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
//		newBindings.ptr->bindings[i] = ns.ptr->bindings.ptr->bindings[i];
//	}
//	for(j = i; j < newSize; ++j) {
//		newBindings.ptr->bindings[j].sym.variant = OCT_SYMBOLOPTION_NOTHING;
//		newBindings.ptr->bindings[j].sym.nothing.dummy = 0;
//		newBindings.ptr->bindings[j].obj.variant = OCT_OBJECTOPTION_NOTHING;
//		newBindings.ptr->bindings[j].obj.nothing.dummy = 0;
//	}
//
//	free(ns.ptr->bindings.ptr);
//	ns.ptr->bindings.ptr = newBindings.ptr;
//
//	// TODO: unlock bindings
//
//ok:
//	ns.ptr->bindings.ptr->bindings[i].sym.variant = OCT_SYMBOLOPTION_SYMBOL;
//	ns.ptr->bindings.ptr->bindings[i].sym.sym = sym;
//    ns.ptr->bindings.ptr->bindings[i].obj = val;
//	return oct_True;
//}
//
//oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BNamespace ns, oct_BSymbol sym, oct_OObjectOption* out_val) {
//	oct_Uword i;
//	oct_BString n1;
//	oct_BString n2;
//	oct_Bool eq;
//
//	// TODO: lock bindings
//
//	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
//		if(ns.ptr->bindings.ptr->bindings[i].sym.variant == OCT_SYMBOLOPTION_SYMBOL) {
//			n1.ptr = ns.ptr->bindings.ptr->bindings[i].sym.sym.ptr->name.ptr;
//			n2.ptr = sym.ptr->name.ptr;
//			if(!oct_BString_equals(ctx, n1, n2, &eq)) {
//				// TODO: unlock bindings
//				return oct_False;
//			}
//			if(eq) {
//				*out_val = ns.ptr->bindings.ptr->bindings[i].obj;
//				// TODO: unlock bindings
//				return oct_True;
//			}
//		}
//	}
//
//	// TODO: unlock bindings
//	out_val->variant = OCT_OBJECTOPTION_NOTHING;
//	out_val->nothing.dummy = 0;
//	return oct_True;
//}
