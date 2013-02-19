#ifndef oct_exchangeheap
#define oct_exchangeheap

#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_ExchangeHeap {
    oct_Uword dummy;
} oct_ExchangeHeap;

struct oct_Context;

oct_Bool oct_ExchangeHeap_allocRaw(struct oct_Context* ctx, oct_Uword size, void** out_box);
oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_BType type, void** out_box);
oct_Bool oct_ExchangeHeap_allocArray(struct oct_Context* ctx, oct_BType type, oct_Uword size, void** out_box);
oct_Bool oct_ExchangeHeap_freeRaw(struct oct_Context* ctx, void* box);
oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box, oct_BType type);

#endif
