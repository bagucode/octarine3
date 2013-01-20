#include "oct_namespace.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"

#include <stddef.h>
#include <stdlib.h>

oct_Bool _oct_NamespaceBinding_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.NamespaceBinding;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_NamespaceBinding);
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_NamespaceBinding, sym);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Symbol;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_NamespaceBinding, obj);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.Any;
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
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_Namespace);
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
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
		// TODO: set OOM in ctx
		// TODO: unlock namespace collection
		return oct_False;
	}
	newNode->name = name;
	newNode->next = NULL;

	// create bindings array, start with 10
	newNode->ns.bindings.ptr = (oct_ANamespaceBinding*)malloc(sizeof(oct_ANamespaceBinding) + (sizeof(oct_NamespaceBinding) * 10));
	if(!newNode->ns.bindings.ptr) {
		free(newNode);
		// TODO: set OOM in ctx
		// TODO: unlock namespace collection
		return oct_False;
	}
	newNode->ns.bindings.ptr->size = 10;

	// init bindings to nothing
	for(i = 0; i < 10; ++i) {
		newNode->ns.bindings.ptr->bindings[i].sym.variant = OCT_NSBINDING_NOTHING;
		newNode->ns.bindings.ptr->bindings[i].sym.nothing.dummy = 0;
		// TODO: set the Any instance to an actual value?
		newNode->ns.bindings.ptr->bindings[i].obj.data[0] = 0;
		newNode->ns.bindings.ptr->bindings[i].obj.data[1] = 0;
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
	return oct_True;
}

oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_OSymbol sym, oct_Any val) {
	// TODO: lock bindings
	// find first free binding in namespace

	// if no free binding exists; expand bindings array

	// TODO: unlock bindings
}

