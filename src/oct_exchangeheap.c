#include "oct_exchangeheap.h"
#include "oct_context.h"
#include "oct_type.h"
#include "oct_type_pointers.h"

#include <stdlib.h>

oct_Bool oct_ExchangeHeap_allocRaw(struct oct_Context* ctx, oct_Uword size, void** out_box) {
	*out_box = malloc(size);
	if(!*out_box) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_BType type, void** out_box) {
	oct_Uword size;
	if(type.ptr->variant == OCT_TYPE_ARRAY) {
		oct_Context_setErrorWithCMessage(ctx, "Use allocArray to allocate space for arrays");
		return oct_False;
	}
	oct_Type_sizeOf(ctx, type, &size);
	return oct_ExchangeHeap_allocRaw(ctx, size, out_box);
}

oct_Bool oct_ExchangeHeap_allocArray(struct oct_Context* ctx, oct_BType type, oct_Uword size, void** out_box) {
	oct_Uword elementSize;
	if(type.ptr->variant != OCT_TYPE_ARRAY) {
		oct_Context_setErrorWithCMessage(ctx, "Use alloc to allocate space for non-arrays");
		return oct_False;
	}
	oct_Type_sizeOf(ctx, type.ptr->arrayType.elementType, &elementSize);
	return oct_ExchangeHeap_allocRaw(ctx, size * elementSize + sizeof(oct_Uword), out_box);
}

oct_Bool oct_ExchangeHeap_freeRaw(struct oct_Context* ctx, void* box) {
	free(box);
	return oct_True;
}

oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box, oct_BType type) {
	// what the heck is the type for here?
	return oct_ExchangeHeap_freeRaw(ctx, box);
}
