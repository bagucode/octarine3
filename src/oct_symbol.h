#ifndef oct_symbol
#define oct_symbol

#include "oct_primitives.h"
#include "oct_string.h"
#include "oct_nothing.h"

typedef struct oct_Symbol {
	oct_OString name;
} oct_Symbol;

typedef struct oct_OSymbol {
	oct_Symbol* ptr;
} oct_OSymbol;

typedef struct oct_BSymbol {
	oct_Symbol* ptr;
} oct_BSymbol;

#define OCT_SYMBOLOPTION_NOTHING 0
#define OCT_SYMBOLOPTION_SYMBOL 1

typedef struct oct_OSymbolOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OSymbol sym;
	};
} oct_OSymbolOption;

// Private

struct oct_Context;

oct_Bool _oct_Symbol_initType(struct oct_Context* ctx);
oct_Bool _oct_OSymbol_initType(struct oct_Context* ctx);
oct_Bool _oct_OSymbolOption_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_Symbol_ctor(struct oct_Context* ctx, oct_Symbol* sym, oct_OString name);
oct_Bool oct_Symbol_dtor(struct oct_Context* ctx, oct_Symbol* sym);
oct_Bool oct_OSymbol_alloc(struct oct_Context* ctx, oct_OString name, oct_OSymbol* out_result);

#endif
