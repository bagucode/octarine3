#include "oct_printable.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_exchangeheap.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Printable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Printable.ptr, "_oct_Printable_initProtocol"));
	ctx->rt->builtInProtocols.Printable.ptr->protocolType = ctx->rt->builtInTypes.Printable;
	table.ptr = &ctx->rt->builtInProtocols.Printable.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Printable_init(struct oct_Context* ctx) {
	oct_CFunction fn;

	// Printable protocol
	oct_CType t = ctx->rt->builtInTypes.Printable;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ACFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.print;

	// OPrintable
	t = ctx->rt->builtInTypes.OPrintable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Printable;

	// BPrintable
	t = ctx->rt->builtInTypes.BPrintable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Printable;

	// print function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.print.ptr, "functions.print"));
	fn = ctx->rt->functions.print;
	CHECK(oct_ACType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ACType_createOwned(ctx, 0, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BSelf;

	return oct_True;
}

// Helper

oct_Bool oct_Printable_print(struct oct_Context* ctx, oct_BPrintable obj) {
	return obj.vtable->functions.print(ctx, obj.self);
}
