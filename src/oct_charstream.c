#include "oct_charstream.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_exchangeheap.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Charstream_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCRAW(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Charstream.ptr, "_oct_Charstream_initProtocol"));
	ctx->rt->builtInProtocols.Charstream.ptr->protocolType = ctx->rt->builtInTypes.Charstream;
	table.ptr = &ctx->rt->builtInProtocols.Charstream.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Charstream_init(struct oct_Context* ctx) {

	// Charstream protocol
	oct_BType t = ctx->rt->builtInTypes.Charstream;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 2, &t.ptr->protocolType.functions));
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

	return oct_True;
}

