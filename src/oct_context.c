#include "oct_context.h"

#include <stddef.h>

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

oct_Bool oct_Context_setErrorOOM(oct_Context* ctx) {
	oct_OString message;
	oct_OError error;
	// TODO: this may also fail because of OOM, need to pre-allocate the OOM error object during runtime startup
	oct_String_createOwnedFromCString(ctx, "Out of memory", &message);
	oct_Error_createOwned(ctx, message, &error);
	return oct_Context_setError(ctx, error);
}

oct_Bool oct_Context_setErrorWithCMessage(oct_Context* ctx, const char* msg) {
	oct_OString message;
	oct_OError error;
	// TODO: this may fail with OOM
	oct_String_createOwnedFromCString(ctx, msg, &message);
	oct_Error_createOwned(ctx, message, &error);
	return oct_Context_setError(ctx, error);
}

oct_Bool oct_Context_clearError(oct_Context* ctx) {
	if(ctx->err.variant == OCT_ERROROPTION_ERROR) {
		oct_Error_destroyOwned(ctx, ctx->err.error);
	}
	ctx->err.variant = OCT_ERROROPTION_NOTHING;
	ctx->err.error.ptr = NULL;
	return oct_True;
}
