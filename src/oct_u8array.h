#ifndef oct_u8array_private
#define oct_u8array_private

#include "oct_primitives.h"

typedef struct oct_AU8 {
	oct_Uword size;
	oct_U8 data[];
} oct_AU8;

typedef struct oct_OAU8 {
	oct_AU8* ptr;
} oct_OAU8;

struct oct_Context;

// Private

oct_Bool _oct_AU8_initType(struct oct_Context* ctx);
oct_Bool _oct_OAU8_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_OAU8_alloc(struct oct_Context* ctx, oct_Uword size, oct_OAU8* out_result);

#endif
