#ifndef oct_copyable
#define oct_copyable

#include "oct_primitives.h"
#include "oct_self.h"
#include "oct_type_pointers.h"

struct oct_Context;

typedef struct oct_CopyableFunctions {
	oct_Bool (*copyOwned)   (struct oct_Context* ctx, oct_BSelf self, oct_OSelf* out_copy);
	//oct_Bool (*copyManaged) (struct oct_Context* ctx, oct_BSelf self, oct_MSelf* out_copy);
} oct_CopyableFunctions;

typedef struct oct_CopyableVTable {
	oct_BType type;
	oct_CopyableFunctions functions;
} oct_CopyableVTable;

typedef struct oct_OCopyable {
	oct_OSelf self;
	oct_CopyableVTable* vtable;
} oct_OCopyable;

oct_BType _oct_OCopyableType;

typedef struct oct_BCopyable {
	oct_BSelf self;
	oct_CopyableVTable* vtable;
} oct_BCopyable;

oct_BType _oct_BCopyableType;

oct_Bool _oct_Copyable_init(struct oct_Context* ctx);

// Helpers

oct_Bool oct_Copyable_copyOwned(struct oct_Context* ctx, oct_BCopyable cp, oct_OObject* out_cp) {
	out_cp->vtable = (oct_ObjectVTable*)cp.vtable;
	return cp.vtable->functions.copyOwned(ctx, cp.self, &out_cp->self);
}

//oct_Bool oct_Copyable_copyManaged(struct oct_Context* ctx, oct_BCopyable cp, oct_MObject* out_cp) {
//	out_cp->vtable = (oct_ObjectVTable*)cp.vtable;
//	return cp.vtable->functions.copyManaged(ctx, cp.self, &out_cp->self);
//}

#endif
