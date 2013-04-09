#ifndef oct_object
#define oct_object

#include "oct_generic.h"
#include "oct_primitives.h"
#include "oct_type_pointers.h"
#include "oct_nothing.h"
#include "oct_protocolbindingpointers.h"

struct oct_Context;

typedef oct_Bool(*oct_Object_dtor)(struct oct_Context* ctx, oct_BGeneric self);

typedef struct oct_ObjectFunctions {
	oct_Object_dtor dtor;
} oct_ObjectFunctions;

typedef struct oct_ObjectVTable {
	oct_CType type;
	oct_ObjectFunctions functions;
} oct_ObjectVTable;

typedef struct oct_BObject {
	oct_BGeneric self;
	oct_ObjectVTable* vtable;
} oct_BObject;

typedef struct oct_OObject {
	oct_OGeneric self;
	oct_ObjectVTable* vtable;
} oct_OObject;

#define OCT_OOBJECTOPTION_NOTHING 0
#define OCT_OOBJECTOPTION_OBJECT 1

typedef struct oct_OObjectOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OObject object;
	};
} oct_OObjectOption;

typedef struct oct_AOObjectOption {
	oct_Uword size;
	oct_OObjectOption data[];
} oct_AOObjectOption;

typedef struct oct_OAOObjectOption {
	oct_AOObjectOption* ptr;
} oct_OAOObjectOption;

typedef struct oct_BAOObjectOption {
	oct_AOObjectOption* ptr;
} oct_BAOObjectOption;

#define OCT_BOBJECTOPTION_NOTHING 0
#define OCT_BOBJECTOPTION_OBJECT 1

typedef struct oct_BObjectOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_BObject object;
	};
} oct_BObjectOption;

oct_Bool _oct_Object_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Object_init(struct oct_Context* ctx);

oct_Bool oct_AOObjectOption_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAOObjectOption* out_arr);
oct_Bool oct_AOObjectOption_dtor(struct oct_Context* ctx, oct_BAOObjectOption self);

// The output is BObject because C does not have templates but the output should be safe to manually cast to the given protocol
oct_Bool oct_Object_as(struct oct_Context* ctx, oct_BGeneric object, oct_CType selfType, oct_BProtocolBinding protocol, oct_BObject* out_casted);

typedef oct_Bool(*oct_PrewalkFn)(struct oct_Context* ctx, oct_OGeneric obj, oct_CType objType, oct_OGeneric* out_result);

oct_Bool oct_Object_preWalk(struct oct_Context* ctx, oct_OGeneric root, oct_CType rootType, oct_PrewalkFn fn);

oct_Bool oct_Object_destroyOwned(struct oct_Context* ctx, oct_OObject obj);

#endif
