#ifndef oct_object_type
#define oct_object_type

#include "oct_object.h"
#include "oct_nothing_type.h"
#include "oct_generic_type.h"
#include "oct_function_type.h"

oct_ACType _oct_ObjectType_dtorParamTypes = {
	1,
	{{(oct_Type*)&_oct_BGenericType}}
};
oct_ACType _oct_ObjectType_dtorReturnTypes = {0};
oct_Function _oct_ObjectType_dtorFunction = {
	{&_oct_ObjectType_dtorParamTypes},
	{&_oct_ObjectType_dtorReturnTypes}
};
oct_ACFunction _oct_ObjectTypeFunctions = {
	1,
	{{&_oct_ObjectType_dtorFunction}}
};
struct {
	oct_Uword v;
	oct_ProtocolType p;
} _oct_ObjectType = {
	OCT_TYPE_PROTOCOL,
	{{&_oct_ObjectTypeFunctions}}
};

OCT_DEF_O_POINTER(OObject, Object);
OCT_DEF_B_POINTER(BObject, Object);

oct_ProtocolBinding _oct_ObjectProtocol;

struct oct_Context;
oct_Bool _oct_Object_initializeProtocol(struct oct_Context* ctx);

#endif
