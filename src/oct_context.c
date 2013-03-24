#include "oct_context.h"
#include "oct_runtime.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Context_init(oct_Context* ctx) {
	// Should this even be used?
	// The context might not be good to expose in-language?

    return oct_True;
}

oct_Bool oct_Context_setError(oct_Context* ctx, oct_ErrorOption err) {
	if(ctx->err.variant == OCT_ERROROPTION_OERROR) {
		oct_Error_destroyOwned(ctx, ctx->err.oerror);
	}
	ctx->err = err;
	return oct_True;
}

oct_Bool oct_Context_getError(oct_Context* ctx, oct_ErrorOption* out_err) {
	*out_err = ctx->err;
	return oct_True;
}

oct_Bool oct_Context_setErrorOOM(oct_Context* ctx) {
	oct_ErrorOption err;
	err.variant = OCT_ERROROPTION_BERROR;
	err.berror = ctx->rt->errors.oom;
	return oct_Context_setError(ctx, err);
}

oct_Bool oct_Context_setErrorWithCMessage(oct_Context* ctx, const char* msg) {
	oct_OString message;
	oct_ErrorOption error;
	CHECK(oct_String_createOwnedFromCString(ctx, msg, &message));
	CHECK(oct_Error_createOwned(ctx, message, &error.oerror));
	error.variant = OCT_ERROROPTION_OERROR;
	return oct_Context_setError(ctx, error);
}

oct_Bool oct_Context_clearError(oct_Context* ctx) {
	if(ctx->err.variant == OCT_ERROROPTION_OERROR) {
		oct_Error_destroyOwned(ctx, ctx->err.oerror);
	}
	ctx->err.variant = OCT_ERROROPTION_NOTHING;
	return oct_True;
}
