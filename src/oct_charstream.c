#include "oct_charstream.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_exchangeheap.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Charstream_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Charstream.ptr, "_oct_Charstream_initProtocol"));
	ctx->rt->builtInProtocols.Charstream.ptr->protocolType = ctx->rt->builtInTypes.Charstream;
	table.ptr = &ctx->rt->builtInProtocols.Charstream.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Charstream_init(struct oct_Context* ctx) {
	oct_CFunction fn;

	// Charstream protocol
	oct_CType t = ctx->rt->builtInTypes.Charstream;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ACFunction_createOwned(ctx, 2, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.readChar;
	t.ptr->protocolType.functions.ptr->data[1] = ctx->rt->functions.peekChar;

	// OCharstream
	t = ctx->rt->builtInTypes.OCharstream;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Charstream;

	// BCharstream
	t = ctx->rt->builtInTypes.BCharstream;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Charstream;

	// readChar function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.readChar.ptr, "functions.readChar"));
	fn = ctx->rt->functions.readChar;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.Char;

	// peekChar function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.peekChar.ptr, "functions.peekChar"));
	fn = ctx->rt->functions.peekChar;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BGeneric;
	fn.ptr->returnTypes.ptr->data[0] = ctx->rt->builtInTypes.Char;

	return oct_True;
}

