#include "oct_symbol.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>

// Private

oct_Bool _oct_Symbol_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Symbol;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_Symbol);
	result = oct_OAField_alloc(ctx, 1, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_Symbol, name);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OString;
	return oct_True;
}

oct_Bool _oct_OSymbol_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OSymbol->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OSymbol->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OSymbol->pointerType.type.ptr = ctx->rt->builtInTypes.Symbol;
	return oct_True;
}

oct_Bool _oct_OSymbolOption_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.OSymbolOption;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_OSymbolOption);
	result = oct_OABType_alloc(ctx, 2, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.Nothing;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.OSymbol;
	return oct_True;
}

// Public

oct_Bool oct_Symbol_ctor(struct oct_Context* ctx, oct_Symbol* sym, oct_OString name) {
	sym->name = name;
	return oct_True;
}

oct_Bool oct_Symbol_dtor(struct oct_Context* ctx, oct_Symbol* sym) {
	return oct_String_destroyOwned(ctx, sym->name);
}

oct_Bool oct_Symbol_createOwned(struct oct_Context* ctx, oct_OString name, oct_OSymbol* out_result) {
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_Symbol), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->name = name;
	return oct_True;
}

oct_Bool oct_Symbol_destroyOwned(struct oct_Context* ctx, oct_OSymbol sym) {
	oct_Bool result = oct_Symbol_dtor(ctx, sym.ptr);
	return oct_ExchangeHeap_free(ctx, sym.ptr) && result;
}
