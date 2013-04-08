#ifndef oct_hashable
#define oct_hashable

#include "oct_self.h"
#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_HashableFunctions {
	oct_Bool (*hash) (struct oct_Context* ctx, oct_BSelf self, oct_Uword* out_hash);
} oct_HashableFunctions;

typedef struct oct_HashableVTable {
	oct_CType type;
	oct_HashableFunctions functions;
} oct_HashableVTable;

//typedef struct oct_BHashable {
//	oct_BSelf self;
//	oct_HashableVTable* vtable;
//} oct_BHashable;
//
//typedef struct oct_MHashable {
//	oct_MSelf self;
//	oct_HashableVTable* vtable;
//} oct_MHashable;
//
//typedef struct oct_OHashable {
//	oct_OSelf self;
//	oct_HashableVTable* vtable;
//} oct_OHashable;

struct oct_Context;

oct_Bool _oct_Hashable_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Hashable_init(struct oct_Context* ctx);

#endif
