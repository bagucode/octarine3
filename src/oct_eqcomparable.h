#ifndef oct_eqcomparable
#define oct_eqcomparable

#include "oct_primitives.h"
#include "oct_generic.h"
#include "oct_type_pointers.h"

struct oct_Context;

typedef oct_Bool(*oct_equals)(struct oct_Context* ctx, oct_BGeneric self, oct_BGeneric other, oct_Bool* out_eq);

typedef struct oct_EqComparableFunctions {
	oct_equals equals;
} oct_EqComparableFunctions;

typedef struct oct_EqComparableVTable {
	oct_CType type;
	oct_EqComparableFunctions functions;
} oct_EqComparableVTable;

//typedef struct oct_BEqComparable {
//	oct_BGeneric self;
//	oct_EqComparableVTable* vtable;
//} oct_BEqComparable;
//
//typedef struct oct_MEqComparable {
//	oct_MGeneric self;
//	oct_EqComparableVTable* vtable;
//} oct_MEqComparable;
//
//typedef struct oct_OEqComparable {
//	oct_OGeneric self;
//	oct_EqComparableVTable* vtable;
//} oct_OEqComparable;

oct_Bool _oct_EqComparable_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_EqComparable_init(struct oct_Context* ctx);

#endif
