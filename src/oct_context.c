#include "oct_context.h"

oct_Bool _oct_Context_initType(oct_Context* ctx) {
	// Should this even be used?
	// The context might not be good to expose in-language?
}

oct_Bool oct_Context_setError(oct_Context* ctx, oct_OError err) {
	if(ctx->err.variant == OCT_ERROROPTION_ERROR) {
		oct_Error_destroyOwned(ctx, ctx->err.error);
	}
	ctx->err.variant = OCT_ERROROPTION_ERROR;
	ctx->err.error.ptr = err.ptr;
	return oct_True;
}

oct_Bool oct_Context_getError(oct_Context* ctx, oct_ErrorOption* out_err) {
	*out_err = ctx->err;
	return oct_True;
}
