#ifndef oct_structtype_private
#define oct_structtype_private

#include "oct_field.h"

typedef struct oct_StructType {
	oct_Uword alignment;
	oct_Uword size;
	oct_OAField fields;
} oct_StructType;

struct oct_Context;

oct_Bool _oct_StructType_initType(struct oct_Context* ctx);

typedef struct _oct_StructInitInfo {
	const char* fieldName;
	oct_BType type;
} _oct_StructInitInfo;

oct_Bool _oct_StructType_construct(struct oct_Context* ctx, _oct_StructInitInfo* infos);

#endif
