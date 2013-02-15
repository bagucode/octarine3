#ifndef oct_namespace
#define oct_namespace

#include "oct_primitives.h"
#include "oct_object.h"
#include "oct_hashtable.h"

struct oct_Context;

typedef struct oct_BindingInfo {
	oct_OHashtableKey key;
	oct_OObject object;
} oct_BindingInfo;

oct_Bool oct_Namespace_bind(struct oct_Context* ctx, const char* ns, oct_BindingInfo* bindings);
oct_Bool oct_Namespace_find(struct oct_Context* ctx, const char* ns, oct_BHashtableKey key, oct_OObject* out_obj);

#endif
