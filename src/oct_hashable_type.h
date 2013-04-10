#ifndef oct_hashable_type
#define oct_hashable_type

#include "oct_hashable.h"
#include "oct_primitive_types.h"
#include "oct_generic_type.h"
#include "oct_function_type.h"

oct_ACType _oct_HashableType_hashParamTypes = {
	1,
	{{(oct_Type*)&_oct_BGenericType}}
};
oct_ACType _oct_HashableType_hashReturnTypes = {
	1,
	{{(oct_Type*)&_oct_UwordType}}
};
oct_Function _oct_HashableType_hashFunction = {
	{&_oct_HashableType_hashParamTypes},
	{&_oct_HashableType_hashReturnTypes}
};
oct_ACFunction _oct_HashableTypeFunctions = {
	1,
	{{&_oct_HashableType_hashFunction}}
};
struct {
	oct_Uword v;
	oct_ProtocolType p;
} _oct_HashableType = {
	OCT_TYPE_PROTOCOL,
	{{&_oct_HashableTypeFunctions}}
};

OCT_DEF_O_POINTER(OHashable, Hashable);
OCT_DEF_B_POINTER(BHashable, Hashable);

oct_ProtocolBinding _oct_HashableProtocol;

struct oct_Context;
oct_Bool _oct_Hashable_initializeProtocol(struct oct_Context* ctx);

#endif
