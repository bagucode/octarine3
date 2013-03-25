#ifndef oct_exchangeheap
#define oct_exchangeheap

#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_ExchangeHeap {
    oct_Uword dummy;
} oct_ExchangeHeap;

struct oct_Context;

#ifdef OCT_DEBUG
#define OCT_ALLOCOWNED(size, out_box, desc) oct_ExchangeHeap_debugAlloc(ctx, size, out_box, desc, __FILE__, __LINE__)
#define OCT_FREEOWNED(box) oct_ExchangeHeap_debugFree(ctx, box)
oct_Bool oct_ExchangeHeap_debugAlloc(struct oct_Context* ctx, oct_Uword size, void** out_box, const char* description, const char* file, int line);
oct_Bool oct_ExchangeHeap_debugFree(struct oct_Context* ctx, void* box);
oct_Bool oct_ExchangeHeap_report(struct oct_Context* ctx);
#else
#define OCT_ALLOCOWNED(size, out_box, desc) oct_ExchangeHeap_alloc(ctx, size, out_box)
#define OCT_FREEOWNED(box) oct_ExchangeHeap_free(ctx, box)
#endif

oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_Uword size, void** out_box);
//oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_BType type, void** out_box);
//oct_Bool oct_ExchangeHeap_allocArray(struct oct_Context* ctx, oct_BType type, oct_Uword size, void** out_box);
oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box);
//oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box, oct_BType type);

#endif
