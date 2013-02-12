#include "oct_namespace.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_string.h"
#include "oct_error.h"

#include <stddef.h>
#include <stdlib.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_NamespaceBinding_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.NamespaceBinding;

	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_NamespaceBinding);
	CHECK(oct_OAField_alloc(ctx, 2, &t->structType.fields));
	t->structType.fields.ptr->data[0].offset = offsetof(oct_NamespaceBinding, sym);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Symbol;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_NamespaceBinding, obj);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.OObjectOption;
	return oct_True;
}

oct_Bool _oct_ANamespaceBinding_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.ANamespaceBinding->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ANamespaceBinding->arrayType.elementType.ptr = ctx->rt->builtInTypes.NamespaceBinding;
	return oct_True;
}

oct_Bool _oct_OANamespaceBinding_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OANamespaceBinding->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OANamespaceBinding->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OANamespaceBinding->pointerType.type.ptr = ctx->rt->builtInTypes.ANamespaceBinding;
	return oct_True;
}

oct_Bool _oct_Namespace_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Namespace;

	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_Namespace);
	CHECK(oct_OAField_alloc(ctx, 1, &t->structType.fields));
	t->structType.fields.ptr->data[0].offset = offsetof(oct_Namespace, bindings);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OANamespaceBinding;
	return oct_True;
}

oct_Bool _oct_BNamespace_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BNamespace->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BNamespace->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BNamespace->pointerType.type.ptr = ctx->rt->builtInTypes.Namespace;
	return oct_True;
}

oct_Bool oct_Namespace_create(struct oct_Context* ctx, oct_OSymbol name, oct_BNamespace* out_ns) {
	oct_NamespaceList *newNode, *lastNode;
	oct_Uword i;

	// TODO: Lock namespace collection
	// TODO: just return existing namespace if there is a name clash
	newNode = (oct_NamespaceList*)malloc(sizeof(oct_NamespaceList));
	if(!newNode) {
		oct_Context_setErrorOOM(ctx);
		// TODO: unlock namespace collection
		return oct_False;
	}
	newNode->name = name;
	newNode->next = NULL;

	// create bindings array, start with 10
	newNode->ns.bindings.ptr = (oct_ANamespaceBinding*)malloc(sizeof(oct_ANamespaceBinding) + (sizeof(oct_NamespaceBinding) * 10));
	if(!newNode->ns.bindings.ptr) {
		free(newNode);
		oct_Context_setErrorOOM(ctx);
		// TODO: unlock namespace collection
		return oct_False;
	}
	newNode->ns.bindings.ptr->size = 10;

	// init bindings to nothing
	for(i = 0; i < 10; ++i) {
		newNode->ns.bindings.ptr->bindings[i].sym.variant = OCT_SYMBOLOPTION_NOTHING;
		newNode->ns.bindings.ptr->bindings[i].sym.nothing.dummy = 0;
		newNode->ns.bindings.ptr->bindings[i].obj.variant = OCT_OBJECTOPTION_NOTHING;
		newNode->ns.bindings.ptr->bindings[i].obj.nothing.dummy = 0;
	}

	// put new node in list
	if(!ctx->rt->namespaces) {
		ctx->rt->namespaces = newNode;
	}
	else {
		lastNode = ctx->rt->namespaces;
		while(lastNode->next) {
			lastNode = lastNode->next;
		}
		lastNode->next = newNode;
	}
	// TODO: unlock namespace collection
	out_ns->ptr = &newNode->ns;
	return oct_True;
}

oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_OSymbol sym, oct_OObjectOption val) {
	oct_Uword i, j;
	oct_Uword newSize;
    oct_Uword ptrKind;
	oct_OANamespaceBinding newBindings;
    
	// TODO: lock bindings
	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
		if(ns.ptr->bindings.ptr->bindings[i].sym.variant == OCT_SYMBOLOPTION_NOTHING) {
			// Free slot
			// TODO: save free slot index but check all because we can't have duplicates!
			goto ok;
		}
	}
	// No free bindings. Expand array.
	newSize = ns.ptr->bindings.ptr->size * 2;
	newBindings.ptr = (oct_ANamespaceBinding*)malloc(sizeof(oct_ANamespaceBinding) + (sizeof(oct_NamespaceBinding) * newSize));
	if(newBindings.ptr == NULL) {
		oct_Context_setErrorOOM(ctx);
		// TODO: unlock bindings
		return oct_False; // OOM
	}
	newBindings.ptr->size = newSize;
	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
		newBindings.ptr->bindings[i] = ns.ptr->bindings.ptr->bindings[i];
	}
	for(j = i; j < newSize; ++j) {
		newBindings.ptr->bindings[j].sym.variant = OCT_SYMBOLOPTION_NOTHING;
		newBindings.ptr->bindings[j].sym.nothing.dummy = 0;
		newBindings.ptr->bindings[j].obj.variant = OCT_OBJECTOPTION_NOTHING;
		newBindings.ptr->bindings[j].obj.nothing.dummy = 0;
	}

	free(ns.ptr->bindings.ptr);
	ns.ptr->bindings.ptr = newBindings.ptr;

	// TODO: unlock bindings

ok:
	ns.ptr->bindings.ptr->bindings[i].sym.variant = OCT_SYMBOLOPTION_SYMBOL;
	ns.ptr->bindings.ptr->bindings[i].sym.sym = sym;
    ns.ptr->bindings.ptr->bindings[i].obj = val;
	return oct_True;
}

oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BNamespace ns, oct_BSymbol sym, oct_OObjectOption* out_val) {
	oct_Uword i;
	oct_BString n1;
	oct_BString n2;
	oct_Bool eq;

	// TODO: lock bindings

	for(i = 0; i < ns.ptr->bindings.ptr->size; ++i) {
		if(ns.ptr->bindings.ptr->bindings[i].sym.variant == OCT_SYMBOLOPTION_SYMBOL) {
			n1.ptr = ns.ptr->bindings.ptr->bindings[i].sym.sym.ptr->name.ptr;
			n2.ptr = sym.ptr->name.ptr;
			if(!oct_BString_equals(ctx, n1, n2, &eq)) {
				// TODO: unlock bindings
				return oct_False;
			}
			if(eq) {
				*out_val = ns.ptr->bindings.ptr->bindings[i].obj;
				// TODO: unlock bindings
				return oct_True;
			}
		}
	}

	// TODO: unlock bindings
	out_val->variant = OCT_OBJECTOPTION_NOTHING;
	out_val->nothing.dummy = 0;
	return oct_True;
}
