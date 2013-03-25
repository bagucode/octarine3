#ifndef oct_symbol
#define oct_symbol

#include "oct_primitives.h"
#include "oct_string.h"
#include "oct_nothing.h"
#include "oct_object.h"
#include "oct_hashtable.h"
#include "oct_printable.h"

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

oct_Bool _oct_Symbol_init(struct oct_Context* ctx);

// Public

oct_Bool oct_Symbol_dtor(struct oct_Context* ctx, oct_BSelf self);

oct_Bool oct_Symbol_createOwned(struct oct_Context* ctx, oct_OString name, oct_OSymbol* out_result);
oct_Bool oct_Symbol_destroyOwned(struct oct_Context* ctx, oct_OSymbol sym);

oct_Bool oct_Symbol_equals(struct oct_Context* ctx, oct_BSymbol self, oct_BSymbol other, oct_Bool* out_eq);
oct_Bool oct_Symbol_hash(struct oct_Context* ctx, oct_BSymbol self, oct_Uword* out_hash);
oct_Bool oct_Symbol_print(struct oct_Context* ctx, oct_BSymbol self);

oct_Bool oct_Symbol_asObject(struct oct_Context* ctx, oct_BSymbol sym, oct_BObject* out_obj);
oct_Bool oct_Symbol_asHashtableKey(struct oct_Context* ctx, oct_BSymbol sym, oct_BHashtableKey* out_key);
oct_Bool oct_Symbol_asPrintable(struct oct_Context* ctx, oct_BSymbol self, oct_BPrintable* out_prn);

#endif
