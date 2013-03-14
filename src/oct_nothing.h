#ifndef oct_nothing_private
#define oct_nothing_private

#include "oct_primitives.h"

typedef struct oct_Nothing {
	oct_Uword dummy;
} oct_Nothing;

typedef struct oct_BNothing {
	oct_Nothing* ptr;
} oct_BNothing;

struct oct_Context;

oct_Bool _oct_Nothing_init(struct oct_Context* ctx);

#endif
