#ifndef oct_eqcomparable_type
#define oct_eqcomparable_type

#include "oct_eqcomparable.h"
#include "oct_primitive_types.h"
#include "oct_generic_type.h"
#include "oct_function_type.h"

oct_ACType _oct_EqComparableType_equalsParamTypes = {
	2,{
		{(oct_Type*)&_oct_BGenericType},
		{(oct_Type*)&_oct_BGenericType}
	}
};
oct_ACType _oct_EqComparableType_equalsReturnTypes = {
	1,
	{{(oct_Type*)&_oct_BoolType}}
};
oct_Function _oct_EqComparableType_equalsFunction = {
	{&_oct_EqComparableType_equalsParamTypes},
	{&_oct_EqComparableType_equalsReturnTypes}
};
oct_ACFunction _oct_EqComparableTypeFunctions = {
	1,
	{{&_oct_EqComparableType_equalsFunction}}
};
struct {
	oct_Uword v;
	oct_ProtocolType p;
} _oct_EqComparableType = {
	OCT_TYPE_PROTOCOL,
	{{&_oct_EqComparableTypeFunctions}}
};

OCT_DEF_O_POINTER(OEqComparable, EqComparable);
OCT_DEF_B_POINTER(BEqComparable, EqComparable);

oct_ProtocolBinding _oct_EqComparableProtocol;

struct oct_Context;
oct_Bool _oct_EqComparable_initializeProtocol(struct oct_Context* ctx);

#endif
