#ifndef oct_structtype_private
#define oct_structtype_private

#include "oct_field.h"

typedef struct oct_StructType {
	oct_Uword alignment;
	oct_Uword size;
	oct_OAField fields;
} oct_StructType;

struct oct_Context;

oct_Bool _oct_StructType_init(struct oct_Context* ctx);

#endif
