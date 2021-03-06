#ifndef oct_seq
#define oct_seq

#include "oct_generic.h"
#include "oct_object.h"

struct oct_Context;

typedef struct oct_SeqFunctions {
	// TODO: some kind of generics or overload. Want to be able to re-use this when value type is not Object
	oct_Bool (*first)(struct oct_Context* ctx, oct_BGeneric self, oct_OObjectOption* out_obj);
	oct_Bool (*rest)(struct oct_Context* ctx, oct_BGeneric self, oct_OGeneric* out_rest);
	oct_Bool (*prepend)(struct oct_Context* ctx, oct_BGeneric self, oct_OObject obj);
	oct_Bool (*append)(struct oct_Context* ctx, oct_BGeneric self, oct_OObject obj);
	oct_Bool (*nth)(struct oct_Context* ctx, oct_BGeneric self, oct_Uword idx, oct_OObjectOption* out_obj);
} oct_SeqFunctions;

typedef struct oct_SeqVTable {
	oct_CType type;
	oct_SeqFunctions functions;
} oct_SeqVTable;

typedef struct oct_BSeq {
	oct_BGeneric self;
	oct_SeqVTable* vtable;
} oct_BSeq;

typedef struct oct_OSeq {
	oct_OGeneric self;
	oct_SeqVTable* vtable;
} oct_OSeq;

oct_Bool _oct_Seq_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Seq_init(struct oct_Context* ctx);

// Helpers

oct_Bool oct_Seq_first(struct oct_Context* ctx, oct_BSeq self, oct_OObjectOption* out_obj);
oct_Bool oct_Seq_rest(struct oct_Context* ctx, oct_BSeq self, oct_OGeneric* out_rest);
oct_Bool oct_Seq_prepend(struct oct_Context* ctx, oct_BSeq self, oct_OObject obj);
oct_Bool oct_Seq_append(struct oct_Context* ctx, oct_BSeq self, oct_OObject obj);
oct_Bool oct_Seq_nth(struct oct_Context* ctx, oct_BSeq self, oct_Uword idx, oct_OObjectOption* out_obj);

#endif
