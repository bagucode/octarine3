#ifndef oct_error
#define oct_error

#include "oct_string.h"
#include "oct_nothing.h"

typedef struct oct_Error {
	oct_OString message;
} oct_Error;

typedef struct oct_OError {
	oct_Error* ptr;
} oct_OError;

typedef struct oct_BError {
	oct_Error* ptr;
} oct_BError;

#define OCT_ERROROPTION_NOTHING 0
#define OCT_ERROROPTION_OERROR 1
#define OCT_ERROROPTION_BERROR 2

typedef struct oct_ErrorOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OError oerror;
		oct_BError berror;
	};
} oct_ErrorOption;

// Private

struct oct_Context;

oct_Bool _oct_Error_init(struct oct_Context* ctx);
oct_Bool _oct_Error_initBuiltInErrors(struct oct_Context* ctx);
oct_Bool _oct_Error_destroyBuiltInErrors(struct oct_Context* ctx);

// Public

//oct_Bool oct_Error_ctor(struct oct_Context* ctx, oct_OString message, oct_Error* err);
oct_Bool oct_Error_createOwned(struct oct_Context* ctx, oct_OString message, oct_OError* out_err);
oct_Bool oct_Error_destroyOwned(struct oct_Context* ctx, oct_OError err);

#endif
