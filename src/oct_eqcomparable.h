#ifndef oct_eqcomparable
#define oct_eqcomparable

#include "oct_primitives.h"
#include "oct_self.h"

struct oct_Context;

typedef struct oct_EqComparableVTable {
	oct_Bool (*equals) (struct oct_Context* ctx, oct_BSelf self, oct_BSelf other, oct_Bool* out_eq);
} oct_EqComparableVTable;

typedef struct oct_BEqComparable {
	oct_BSelf self;
	oct_EqComparableVTable* vtable;
} oct_BEqComparable;

typedef struct oct_MEqComparable {
	oct_MSelf self;
	oct_EqComparableVTable* vtable;
} oct_MEqComparable;

typedef struct oct_OEqComparable {
	oct_OSelf self;
	oct_EqComparableVTable* vtable;
} oct_OEqComparable;

oct_Bool _oct_EqComparable_init(struct oct_Context* ctx);

#endif