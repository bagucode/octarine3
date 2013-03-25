#include "oct_copyable.h"
#include "oct_hashtable.h"
#include "oct_runtime.h"
#include "oct_context.h"
#include "oct_type_pointers.h"
#include "oct_type.h"
#include "oct_exchangeheap.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Copyable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Copyable.ptr, "_oct_Copyable_initProtocol"));
	ctx->rt->builtInProtocols.Copyable.ptr->protocolType = ctx->rt->builtInTypes.Copyable;
	table.ptr = &ctx->rt->builtInProtocols.Copyable.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Copyable_init(struct oct_Context* ctx) {
	oct_BFunction fn;

	oct_BType t = ctx->rt->builtInTypes.Copyable;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.copyOwned;

	// copyOwned function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.copyOwned.ptr, "functions.copyOwned"));
	fn = ctx->rt->functions.copyOwned;
	CHECK(oct_ABType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ABType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BSelf;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.OSelf;

	return oct_True;
}

oct_Bool oct_Copyable_copyOwned(struct oct_Context* ctx, oct_BCopyable cp, oct_OObject* out_cp) {
	return cp.vtable->functions.copyOwned(ctx, cp.self, &out_cp->self);
}
