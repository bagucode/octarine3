#include "oct_seqable.h"
#include "oct_hashtable.h"
#include "oct_exchangeheap.h"
#include "oct_runtime.h"
#include "oct_context.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Seqable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Seqable.ptr, "_oct_Seqable_initProtocol"));
	ctx->rt->builtInProtocols.Seqable.ptr->protocolType = ctx->rt->builtInTypes.Seqable;
	table.ptr = &ctx->rt->builtInProtocols.Seqable.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Seqable_init(struct oct_Context* ctx) {
	oct_CFunction fn;

	// Seqable protocol
	oct_CType t = ctx->rt->builtInTypes.Seqable;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ACFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.seq;

	// OSeqable
	t = ctx->rt->builtInTypes.OSeqable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Seqable;

	// BSeqable
	t = ctx->rt->builtInTypes.BSeqable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Seqable;

	// seq function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.seq.ptr, "functions.seq"));
	fn = ctx->rt->functions.seq;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.BSeq;

	return oct_True;
}

oct_Bool oct_Seqable_seq(struct oct_Context* ctx, oct_BSeqable self, oct_BSeq* out_seq) {
	return self.vtable->functions.seq(ctx, self.self, out_seq);
}

