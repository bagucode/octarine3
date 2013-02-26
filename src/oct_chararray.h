#ifndef oct_chararray
#define oct_chararray

#include "oct_primitives.h"

typedef struct oct_AChar {
	oct_Uword size;
	oct_Char data[];
} oct_AChar;

typedef struct oct_OAChar {
	oct_AChar* ptr;
} oct_OAChar;

struct oct_Context;

// Private

oct_Bool _oct_AChar_init(struct oct_Context* ctx);

// Public

oct_Bool oct_AChar_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAChar* out_result);

#endif
