#ifndef oct_namespace
#define oct_namespace

#include "oct_primitives.h"
#include "oct_any.h"
#include "oct_hashtable.h"
#include "oct_string.h"
#include "oct_symbol.h"
#include "oct_nothing.h"
#include "oct_generic.h"

struct oct_Context;
struct oct_Namespace;

typedef struct oct_BNamespace {
	struct oct_Namespace* ptr;
} oct_BNamespace;

#define OCT_NAMESPACEOPTION_NOTHING 0
#define OCT_NAMESPACEOPTION_NAMESPACE 1

typedef struct oct_NamespaceOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_BNamespace ns;
	};
} oct_NamespaceOption;

oct_Bool _oct_Namespace_init(struct oct_Context* ctx);

oct_Bool oct_Namespace_create(struct oct_Context* ctx, oct_OString nsName, oct_BNamespace* out_ns);
oct_Bool oct_Namespace_findNs(struct oct_Context* ctx, oct_BString nsName, oct_NamespaceOption* out_ns);
oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_HashtableKeyOption key, oct_Any value);
oct_Bool oct_Namespace_bindInCurrent(struct oct_Context* ctx, oct_HashtableKeyOption key, oct_Any value);
oct_Bool oct_Namespace_lookup(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_Any* out_value);
oct_Bool oct_Namespace_asObject(struct oct_Context* ctx, oct_BNamespace ns, oct_BObject* out_obj);
oct_Bool oct_Namespace_dtor(struct oct_Context* ctx, oct_BGeneric self);

#endif
