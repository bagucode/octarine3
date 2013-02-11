// Base protocol that all types must implement
// Default implementations are generated for user-created types unless overridden.

#ifndef oct_object
#define oct_object

#include "oct_primitives.h"
#include "oct_nothing.h"

// Private

struct oct_Context;

oct_Bool _oct_Object_initType(struct oct_Context* ctx);
oct_Bool _oct_BObject_initType(struct oct_Context* ctx);
oct_Bool _oct_OObject_initType(struct oct_Context* ctx);
oct_Bool _oct_MObject_initType(struct oct_Context* ctx);
oct_Bool _oct_OObjectOption_initType(struct oct_Context* ctx);

// Public

struct oct_ObjectVTable;

typedef struct oct_Object {
	void* object;
	struct oct_ObjectVTable* vtable;
} oct_Object;

typedef struct oct_BObject {
	oct_Object object;
} oct_BObject;

typedef struct oct_OObject {
	oct_Object object;
} oct_OObject;

typedef struct oct_MObject {
	oct_Object object;
} oct_MObject;

#define OCT_OOBJECTOPTION_NOTHING 0
#define OCT_OOBJECTOPTION_OBJECT 1

typedef struct oct_OObjectOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OObject object;
	};
} oct_OObjectOption;

oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);

#endif
