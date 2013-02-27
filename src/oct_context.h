#ifndef oct_context
#define oct_context

#include "oct_primitives.h"
#include "oct_error.h"

struct oct_Runtime;
struct oct_Reader;
struct oct_Namespace;

typedef struct oct_Context {
	struct oct_Runtime* rt;
	struct oct_Reader* reader;
	struct oct_Namespace* ns;
	oct_OErrorOption err;
} oct_Context;

// Private

oct_Bool _oct_Context_init(oct_Context* ctx);

// Public

oct_Bool oct_Context_setError(oct_Context* ctx, oct_OError err);
oct_Bool oct_Context_setErrorWithCMessage(oct_Context* ctx, const char* msg);
oct_Bool oct_Context_getError(oct_Context* ctx, oct_OErrorOption* out_err);
oct_Bool oct_Context_clearError(oct_Context* ctx);

// Common errors

oct_Bool oct_Context_setErrorOOM(oct_Context* ctx);

#endif
