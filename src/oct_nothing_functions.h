#ifndef oct_nothingfunctions
#define oct_nothingfunctions

#include "oct_primitives.h"
#include "oct_nothing.h"
#include "oct_object.h"
#include "oct_hashtable.h"

oct_Bool oct_Nothing_equals(struct oct_Context* ctx, oct_BNothing self, oct_BNothing other, oct_Bool* out_eq);
oct_Bool oct_Nothing_hash(struct oct_Context* ctx, oct_BNothing self, oct_Uword* out_hash);

oct_Bool oct_Nothing_asObject(struct oct_Context* ctx, oct_BNothing nothing, oct_BObject* out_obj);
oct_Bool oct_Nothing_asHashtableKey(struct oct_Context* ctx, oct_BNothing nothing, oct_BHashtableKey* out_key);

#endif
