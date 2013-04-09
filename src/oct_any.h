#ifndef oct_any
#define oct_any

#include "oct_nothing.h"
#include "oct_object.h"

#define OCT_ANY_NOTHING 0
#define OCT_ANY_OOBJECT 1
#define OCT_ANY_BOBJECT 2
//#define OCT_ANY_MOBJECT 3

typedef struct oct_Any {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OObject oobject;
		oct_BObject bobject;
		//oct_MObject mobject;
	};
} oct_Any;

typedef struct oct_BAny {
	oct_Any* ptr;
} oct_BAny;

struct oct_Context;
oct_Bool _oct_Any_init(struct oct_Context* ctx);

oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_BAny self);

#endif
