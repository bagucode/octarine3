#ifndef oct_namespace
#define oct_namespace

#include "oct_primitives.h"
#include "oct_symbol.h"
#include "oct_any.h"

typedef struct oct_NamespaceBinding {
	oct_OSymbolOption sym;
	oct_AnyOption obj;
} oct_NamespaceBinding;

typedef struct oct_ANamespaceBinding {
	oct_Uword size;
	oct_NamespaceBinding bindings[];
} oct_ANamespaceBinding;

#define OCT_NSBINDING_NOTHING 0
#define OCT_NSBINDING_SOMETHING 1

typedef struct oct_OANamespaceBinding {
	oct_ANamespaceBinding* ptr;
} oct_OANamespaceBinding;

typedef struct oct_Namespace {
	// Just use an array of bindings for now.
	// No need for efficiency in a proof of concept, right? :P
	// Ok, I'm too lazy to make a hash table right now.
	oct_OANamespaceBinding bindings;
	// TODO: lock
} oct_Namespace;

typedef struct oct_BNamespace {
	oct_Namespace* ptr;
} oct_BNamespace;

// Private

struct oct_Context;

oct_Bool _oct_NamespaceBinding_initType(struct oct_Context* ctx);
oct_Bool _oct_ANamespaceBinding_initType(struct oct_Context* ctx);
oct_Bool _oct_OANamespaceBinding_initType(struct oct_Context* ctx);
oct_Bool _oct_Namespace_initType(struct oct_Context* ctx);
oct_Bool _oct_BNamespace_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_Namespace_create(struct oct_Context* ctx, oct_OSymbol name, oct_BNamespace* out_ns);
oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_OSymbol sym, oct_AnyOption val);
oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BNamespace ns, oct_BSymbol sym, oct_AnyOption* out_val);

#endif
