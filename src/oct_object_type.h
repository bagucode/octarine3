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
	{&_oct_ObjectType_dtorFunction}
};
struct {
	oct_Uword v;
	oct_ProtocolType p;
} _oct_ObjectType = {
	OCT_TYPE_PROTOCOL,
	{&_oct_ObjectTypeFunctions}
};

#endif
