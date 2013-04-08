#ifndef oct_copyable
#define oct_copyable

#include "oct_primitives.h"
#include "oct_self.h"
#include "oct_type_pointers.h"
#include "oct_object.h"

struct oct_Context;

typedef struct oct_CopyableFunctions {
	oct_Bool (*copyOwned)   (struct oct_Context* ctx, oct_BSelf self, oct_OSelf* out_copy);
	//oct_Bool (*copyManaged) (struct oct_Context* ctx, oct_BSelf self, oct_MSelf* out_copy);
} oct_CopyableFunctions;

typedef struct oct_CopyableVTable {
	oct_CType type;
	oct_CopyableFunctions functions;
} oct_CopyableVTable;

typedef struct oct_OCopyable {
	oct_OSelf self;
	oct_CopyableVTable* vtable;
} oct_OCopyable;

oct_CType _oct_OCopyableType;

typedef struct oct_BCopyable {
	oct_BSelf self;
	oct_CopyableVTable* vtable;
} oct_BCopyable;

oct_CType _oct_BCopyableType;

oct_Bool _oct_Copyable_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Copyable_init(struct oct_Context* ctx);

// Helpers

oct_Bool oct_Copyable_copyOwned(struct oct_Context* ctx, oct_BCopyable cp, oct_OObject* out_cp);

#endif
