#ifndef oct_namespace
#define oct_namespace

#include "oct_primitives.h"
#include "oct_object.h"
#include "oct_hashtable.h"
#include "oct_string.h"

struct oct_Context;
struct oct_Namespace;

typedef struct oct_BNamespace {
	struct oct_Namespace* ptr;
} oct_BNamespace;

typedef struct oct_BindingInfo {
	oct_OHashtableKey key;
	oct_OObject object;
} oct_BindingInfo;

typedef struct oct_ABindingInfo {
	oct_Uword size;
	oct_BindingInfo data[];
} oct_ABindingInfo;

typedef struct oct_OABindingInfo {
	oct_ABindingInfo* ptr;
} oct_OABindingInfo;

oct_Bool oct_Namespace_findNs(struct oct_Context* ctx, oct_BString nsName, oct_BNamespace* out_ns);
oct_Bool oct_Namespace_bind(struct oct_Context* ctx, oct_BNamespace ns, oct_BindingInfo binding);
oct_Bool oct_Namespace_copyValueOwned(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_OObject* out_obj);
//oct_Bool oct_Namespace_copyValueManaged(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_MObject* out_obj);
oct_Bool oct_Namespace_borrowGlobal(struct oct_Context* ctx, oct_BNamespace ns, oct_BHashtableKey key, oct_BObject* out_obj);

#endif
