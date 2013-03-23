#include "oct_eqcomparable.h"
#include "oct_hashtable.h"
#include "oct_exchangeheap.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_protocoltype.h"
#include "oct_type_pointers.h"
#include "oct_type.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_EqComparable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCRAW(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.EqComparable.ptr, "_oct_EqComparable_initProtocol"));
	ctx->rt->builtInProtocols.EqComparable.ptr->protocolType = ctx->rt->builtInTypes.EqComparable;
	table.ptr = &ctx->rt->builtInProtocols.EqComparable.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_EqComparable_init(struct oct_Context* ctx) {
	oct_BFunction fn;
	oct_BType t = ctx->rt->builtInTypes.EqComparable;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.eq;

	// eq function signature
	CHECK(OCT_ALLOCRAW(sizeof(oct_Function), (void**)&ctx->rt->functions.eq.ptr, "functions.eq"));
	fn = ctx->rt->functions.eq;
	CHECK(oct_ABType_createOwned(ctx, 2, &fn.ptr->paramTypes));
	CHECK(oct_ABType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BSelf;
	fn.ptr->paramTypes.ptr->data[1] = ctx->rt->builtInTypes.BSelf;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.Bool;

	return oct_True;
}

