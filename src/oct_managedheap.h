#ifndef oct_managedheap
#define oct_managedheap

#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_ManagedHeap {
    oct_Uword dummy;
} oct_ManagedHeap;

struct oct_Context;

oct_Bool oct_ManagedHeap_alloc(struct oct_Context* ctx, oct_CType type, void** out_box);
oct_Bool oct_ManagedHeap_pushroot(struct oct_Context* ctx, void* box);
oct_Bool oct_ManagedHeap_poproot(struct oct_Context* ctx);
oct_Bool oct_ManagedHeap_forceGC(struct oct_Context* ctx);

#endif
