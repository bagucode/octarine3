#ifndef oct_object
#define oct_object

#include "oct_self.h"
#include "oct_primitives.h"
#include "oct_type_pointers.h"
#include "oct_nothing.h"
#include "oct_protocolbindingpointers.h"

typedef struct oct_ObjectVTable {
	oct_BType type;
} oct_ObjectVTable;

typedef struct oct_BObject {
	oct_BSelf self;
	oct_ObjectVTable* vtable;
} oct_BObject;

typedef struct oct_OObject {
	oct_OSelf self;
	oct_ObjectVTable* vtable;
} oct_OObject;

#define OCT_OBJECTOPTION_NOTHING 0
#define OCT_OBJECTOPTION_OBJECT 1

typedef struct oct_ObjectOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OObject object;
	};
} oct_ObjectOption;

struct oct_Context;

oct_Bool _oct_Object_init(struct oct_Context* ctx);

// The output is BObject because C does not have templates but the output should be safe to manually cast to the given protocol
oct_Bool oct_Object_as(struct oct_Context* ctx, oct_BSelf object, oct_BType selfType, oct_BProtocolBinding protocol, oct_BObject* out_casted);

typedef oct_Bool(*oct_PrewalkFn)(struct oct_Context* ctx, oct_OSelf obj, oct_BType objType, oct_OSelf* out_result);

oct_Bool oct_Object_preWalk(struct oct_Context* ctx, oct_OSelf root, oct_BType rootType, oct_PrewalkFn fn);

oct_Bool oct_Object_destroyOwned(struct oct_Context* ctx, oct_OObject obj);

#endif
