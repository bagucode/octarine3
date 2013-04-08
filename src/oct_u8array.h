#ifndef oct_u8array_private
#define oct_u8array_private

#include "oct_primitives.h"
#include "oct_generic.h"

typedef struct oct_AU8 {
	oct_Uword size;
	oct_U8 data[];
} oct_AU8;

typedef struct oct_OAU8 {
	oct_AU8* ptr;
} oct_OAU8;

typedef struct oct_BAU8 {
	oct_AU8* ptr;
} oct_BAU8;

struct oct_Context;

// Private

oct_Bool _oct_AU8_init(struct oct_Context* ctx);

// Public

oct_Bool oct_AU8_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAU8* out_result);

oct_Bool oct_AU8_hash(struct oct_Context* ctx, oct_BAU8 self, oct_Uword* out_hash);

oct_Bool oct_AU8_copyOwned(struct oct_Context* ctx, oct_BGeneric orig, oct_OGeneric* out_copy);

#endif

