#ifndef oct_hashtablekey_type
#define oct_hashtablekey_type

#include "oct_hashtable.h"
#include "oct_hashable_type.h"
#include "oct_eqcomparable_type.h"

oct_ACFunction _oct_HashtableKeyTypeFunctions = {
	2, {
		{&_oct_HashableType_hashFunction},
		{&_oct_EqComparableType_equalsFunction}
	}
};
struct {
	oct_Uword v;
	oct_ProtocolType p;
} _oct_HashtableKeyType = {
	OCT_TYPE_PROTOCOL,
	{{&_oct_HashtableKeyTypeFunctions}}
};

OCT_DEF_O_POINTER(OHashtableKey, HashtableKey);
OCT_DEF_B_POINTER(BHashtableKey, HashtableKey);

oct_ProtocolBinding _oct_HashtableKeyProtocol;

struct oct_Context;
oct_Bool _oct_HashtableKey_initializeProtocol(struct oct_Context* ctx);

#endif
