// Base protocol that all types must implement
// Default implementations are generated for user-created types unless overridden.

#ifndef oct_object
#define oct_object

#include "oct_primitives.h"
#include "oct_type_pointers.h"
#include "oct_list.h"

// Private

struct oct_Context;

oct_Bool _oct_Object_initType(struct oct_Context* ctx);

// Public

struct oct_ObjectVTable;

typedef struct oct_Object {
	void* object; // Borrowed?
	struct oct_ObjectVTable* vtable;
} oct_Object;

typedef struct oct_ObjectVTable {
	oct_BType instanceType;
	oct_Bool(*ctor)       (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*dtor)       (struct oct_Context* ctx, void* object);
	//oct_Bool(*print)      (struct oct_Context* ctx, void* object, /*Text output stream*/);
	oct_Bool(*invoke)     (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*copyOwned)  (struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*copyManaged)(struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*hash)       (struct oct_Context* ctx, void* object, oct_Uword* out_hash);
	oct_Bool(*equals)     (struct oct_Context* ctx, void* object, oct_Object other);
} oct_ObjectVTable;

#endif
