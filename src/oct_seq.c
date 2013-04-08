#include "oct_seq.h"
#include "oct_hashtable.h"
#include "oct_exchangeheap.h"
#include "oct_runtime.h"
#include "oct_context.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Seq_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Seq.ptr, "_oct_Seq_initProtocol"));
	ctx->rt->builtInProtocols.Seq.ptr->protocolType = ctx->rt->builtInTypes.Seq;
	table.ptr = &ctx->rt->builtInProtocols.Seq.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Seq_init(struct oct_Context* ctx) {
	oct_CFunction fn;

	// Seq protocol
	oct_CType t = ctx->rt->builtInTypes.Seq;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ACFunction_createOwned(ctx, 5, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.first;
	t.ptr->protocolType.functions.ptr->data[1] = ctx->rt->functions.rest;
	t.ptr->protocolType.functions.ptr->data[2] = ctx->rt->functions.prepend;
	t.ptr->protocolType.functions.ptr->data[3] = ctx->rt->functions.append;
	t.ptr->protocolType.functions.ptr->data[4] = ctx->rt->functions.nth;

	// OSeq
	t = ctx->rt->builtInTypes.OSeq;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Seq;

	// BSeq
	t = ctx->rt->builtInTypes.BSeq;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Seq;

	// first function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.first.ptr, "functions.first"));
	fn = ctx->rt->functions.first;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.OObjectOption;

	// rest function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.rest.ptr, "functions.rest"));
	fn = ctx->rt->functions.rest;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.OGeneric;

	// prepend function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.prepend.ptr, "functions.prepend"));
	fn = ctx->rt->functions.prepend;
	CHECK(oct_ACType_createOwned(ctx, 2, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 0, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->paramTypes.ptr->data[1] = ctx->rt->builtInTypes.OObject;

	// append function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.append.ptr, "functions.append"));
	fn = ctx->rt->functions.append;
	CHECK(oct_ACType_createOwned(ctx, 2, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 0, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->paramTypes.ptr->data[1] = ctx->rt->builtInTypes.OObject;

	// nth function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.nth.ptr, "functions.nth"));
	fn = ctx->rt->functions.nth;
	CHECK(oct_ACType_createOwned(ctx, 2, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->paramTypes.ptr->data[1] = ctx->rt->builtInTypes.Uword;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.OObjectOption;

	return oct_True;
}

oct_Bool oct_Seq_first(struct oct_Context* ctx, oct_BSeq self, oct_OObjectOption* out_obj) {
	return self.vtable->functions.first(ctx, self.self, out_obj);
}

oct_Bool oct_Seq_rest(struct oct_Context* ctx, oct_BSeq self, oct_OGeneric* out_rest) {
	return self.vtable->functions.rest(ctx, self.self, out_rest);
}

oct_Bool oct_Seq_prepend(struct oct_Context* ctx, oct_BSeq self, oct_OObject obj) {
	return self.vtable->functions.prepend(ctx, self.self, obj);
}

oct_Bool oct_Seq_append(struct oct_Context* ctx, oct_BSeq self, oct_OObject obj) {
	return self.vtable->functions.append(ctx, self.self, obj);
}

oct_Bool oct_Seq_nth(struct oct_Context* ctx, oct_BSeq self, oct_Uword idx, oct_OObjectOption* out_obj) {
	return self.vtable->functions.nth(ctx, self.self, idx, out_obj);
}
