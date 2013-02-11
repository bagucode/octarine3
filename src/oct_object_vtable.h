#ifndef oct_object_vtable
#define oct_object_vtable

#include "oct_object.h"
#include "oct_list.h"

typedef struct oct_ObjectVTable {
	oct_BType instanceType;
	oct_Bool(*ctor)       (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*dtor)       (struct oct_Context* ctx, void* object);
	//oct_Bool(*print)      (struct oct_Context* ctx, void* object, /*Text output stream*/);
	oct_Bool(*invoke)     (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*copyOwned)  (struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*copyManaged)(struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*hash)       (struct oct_Context* ctx, void* object, oct_Uword* out_hash);
	oct_Bool(*equals)     (struct oct_Context* ctx, void* object, oct_BObject other);
} oct_ObjectVTable;

#endif
