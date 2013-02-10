#include "oct_exchangeheap.h"
#include "oct_context.h"

#include <stdlib.h>

oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_Uword size, void** out_box) {
	*out_box = malloc(size);
	if(!*out_box) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box) {
	free(box);
	return oct_True;
}
