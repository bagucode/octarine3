#ifndef oct_prototype_private
#define oct_prototype_private

#include "oct_primitives.h"

typedef struct oct_ProtoType {
	oct_Uword dummy;
} oct_ProtoType;

// Private

struct oct_Context;

oct_Bool _oct_ProtoType_initType(struct oct_Context* ctx);

#endif
