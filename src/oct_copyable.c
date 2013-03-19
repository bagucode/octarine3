#include "oct_copyable.h"

oct_Bool oct_Copyable_copyOwned(struct oct_Context* ctx, oct_BCopyable cp, oct_OObject* out_cp) {
	out_cp->vtable = (oct_ObjectVTable*)cp.vtable;
	return cp.vtable->functions.copyOwned(ctx, cp.self, &out_cp->self);
}
