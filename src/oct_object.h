#ifndef oct_object
#define oct_object

#include "oct_self.h"
#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_ObjectVTable {
	oct_Bool (*typeOf) (struct oct_Context* ctx, oct_BSelf self, oct_BType* out_type);
} oct_ObjectVTable;

typedef struct oct_BObject {
	oct_BSelf self;
	oct_ObjectVTable* vtable;
} oct_BObject;

typedef struct oct_OObject {
	oct_OSelf self;
	oct_ObjectVTable* vtable;
} oct_OObject;

struct oct_Context;

oct_Bool _oct_Object_init(struct oct_Context* ctx);

#endif
