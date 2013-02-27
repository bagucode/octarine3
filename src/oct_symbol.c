#include "oct_symbol.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

// Private

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Symbol_init(oct_Context* ctx) {

	// Symbol
	oct_BType t = ctx->rt->builtInTypes.Symbol;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Symbol);
	CHECK(oct_OAField_alloc(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Symbol, name);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OString;

	// Symbol VTable for Object {}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 0, &ctx->rt->vtables.SymbolAsObject, t));

	// Symbol VTable for EqComparable {eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.EqComparable, 1, &ctx->rt->vtables.SymbolAsEqComparable, t, oct_Symbol_equals));

	// Symbol VTable for Hashable {hash}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Hashable, 1, &ctx->rt->vtables.SymbolAsHashable, t, oct_Symbol_hash));

	// Symbol VTable for HashtableKey {hash, eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.HashtableKey, 2, &ctx->rt->vtables.SymbolAsHashtableKey, t, oct_Symbol_hash, oct_Symbol_equals));

	// OSymbol
	ctx->rt->builtInTypes.OSymbol.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OSymbol.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OSymbol.ptr->pointerType.type = ctx->rt->builtInTypes.Symbol;

	// BSymbol
	ctx->rt->builtInTypes.BSymbol.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BSymbol.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BSymbol.ptr->pointerType.type = ctx->rt->builtInTypes.Symbol;

	// OSymbolOption
	t = ctx->rt->builtInTypes.OSymbolOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OSymbolOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OSymbol;

	return oct_True;
}

// Public

oct_Bool oct_Symbol_createOwned(struct oct_Context* ctx, oct_OString name, oct_OSymbol* out_result) {
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_Symbol), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->name = name;
	return oct_True;
}

oct_Bool oct_Symbol_destroyOwned(struct oct_Context* ctx, oct_OSymbol sym) {
	oct_Bool result = oct_String_destroyOwned(ctx, sym.ptr->name);
	return oct_ExchangeHeap_freeRaw(ctx, sym.ptr) && result;
}

oct_Bool oct_Symbol_asObject(struct oct_Context* ctx, oct_BSymbol sym, oct_BObject* out_obj) {
	out_obj->self.self = sym.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.SymbolAsObject.ptr;
	return oct_True;
}

oct_Bool oct_Symbol_asHashtableKey(struct oct_Context* ctx, oct_BSymbol sym, oct_BHashtableKey* out_key) {
	out_key->self.self = sym.ptr;
	out_key->vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.SymbolAsHashtableKey.ptr;
	return oct_True;
}

oct_Bool oct_Symbol_equals(struct oct_Context* ctx, oct_BSymbol self, oct_BSymbol other, oct_Bool* out_eq) {
	oct_BString x;
	oct_BString y;
	x.ptr = self.ptr->name.ptr;
	y.ptr = other.ptr->name.ptr;
	return oct_BString_equals(ctx, x, y, out_eq);
}

oct_Bool oct_Symbol_hash(struct oct_Context* ctx, oct_BSymbol self, oct_Uword* out_hash) {
	oct_BString x;
	x.ptr = self.ptr->name.ptr;
	CHECK(oct_String_hash(ctx, x, out_hash));
	(*out_hash) *= 131;
	return oct_True;
}
