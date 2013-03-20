#include "oct_hashable.h"
#include "oct_hashtable.h"
#include "oct_exchangeheap.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_protocoltype.h"
#include "oct_type_pointers.h"
#include "oct_type.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Hashable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Hashable.ptr));
	ctx->rt->builtInProtocols.Hashable.ptr->protocolType = ctx->rt->builtInTypes.Hashable;
	table.ptr = &ctx->rt->builtInProtocols.Hashable.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Hashable_init(struct oct_Context* ctx) {
	oct_BType t = ctx->rt->builtInTypes.Hashable;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.hash;

	return oct_True;
}
