#ifndef oct_seqable
#define oct_seqable

#include "oct_generic.h"
#include "oct_object.h"
#include "oct_seq.h"

struct oct_Context;

typedef struct oct_SeqableFunctions {
	oct_Bool (*seq)(struct oct_Context* ctx, oct_BGeneric self, oct_BSeq* out_seq);
} oct_SeqableFunctions;

typedef struct oct_SeqableVTable {
	oct_CType type;
	oct_SeqableFunctions functions;
} oct_SeqableVTable;

typedef struct oct_BSeqable {
	oct_BGeneric self;
	oct_SeqableVTable* vtable;
} oct_BSeqable;

typedef struct oct_OSeqable {
	oct_OGeneric self;
	oct_SeqableVTable* vtable;
} oct_OSeqable;

oct_Bool _oct_Seqable_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Seqable_init(struct oct_Context* ctx);

// Helpers

oct_Bool oct_Seqable_seq(struct oct_Context* ctx, oct_BSeqable self, oct_BSeq* out_seq);

#endif
