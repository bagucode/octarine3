// Base protocol that all types must implement
// Default implementations are generated for user-created types unless overridden.

#ifndef oct_object
#define oct_object

#include "oct_primitives.h"
#include "oct_type_pointers.h"
#include "oct_nothing.h"

// Private

struct oct_Context;

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

typedef struct oct_ObjectVTable {
	oct_BType instanceType;
	//oct_Bool(*ctor)       (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*dtor)       (struct oct_Context* ctx, void* object);
	//oct_Bool(*print)      (struct oct_Context* ctx, void* object, /*Text output stream*/);
	//oct_Bool(*invoke)     (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*copyOwned)  (struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*copyManaged)(struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*hash)       (struct oct_Context* ctx, void* object, oct_Uword* out_hash);
	oct_Bool(*equals)     (struct oct_Context* ctx, void* object, oct_BObject other);
} oct_ObjectVTable;

oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);

#endif
