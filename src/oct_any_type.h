#ifndef oct_any_type
#define oct_any_type

#include "oct_any.h"
#include "oct_nothing_type.h"
#include "oct_object_type.h"

// Any
oct_ACType _oct_AnyTypes = {
	3, {
		{(oct_Type*)&_oct_NothingType},
		{(oct_Type*)&_oct_OObjectType},
		{(oct_Type*)&_oct_BObjectType}
	}
};
struct {
	oct_Uword v;
	oct_VariadicType vt;
} _oct_AnyType = {
	OCT_TYPE_VARIADIC,
	{ 0, sizeof(oct_Any), {&_oct_AnyTypes}}
};

OCT_DEF_B_POINTER(BAny, Any);

#endif
