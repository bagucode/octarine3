#ifndef oct_object_vtable
#define oct_object_vtable

#include "oct_object.h"
#include "oct_list.h"
#include "oct_type_pointers.h"

typedef struct oct_ObjectVTable {
	oct_BType instanceType;
	oct_Bool(*ctor)       (struct oct_Context* ctx, void* object, oct_OList args);
	oct_Bool(*dtor)       (struct oct_Context* ctx, void* object);
	//oct_Bool(*print)      (struct oct_Context* ctx, void* object, /*Text output stream*/);
	oct_Bool(*invoke)     (struct oct_Context* ctx, void* object, oct_OList args);
	//oct_Bool(*eval)       (struct oct_Context* ctx, void* object);
	oct_Bool(*copyOwned)  (struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*copyManaged)(struct oct_Context* ctx, void* object, void** out_copy);
	oct_Bool(*hash)       (struct oct_Context* ctx, void* object, oct_Uword* out_hash);
	oct_Bool(*equals)     (struct oct_Context* ctx, void* object, oct_BObject other, oct_Bool* out_result);
} oct_ObjectVTable;

// Helper functions for easier calling of the vtable functions

oct_Bool oct_Object_ctor(struct oct_Context* ctx, oct_Object obj, oct_OList args);
oct_Bool oct_Object_dtor(struct oct_Context* ctx, oct_Object obj);
//oct_Bool oct_Object_print(struct oct_Context* ctx, oct_Object obj, /*Text output stream*/);
oct_Bool oct_Object_invoke(struct oct_Context* ctx, oct_Object obj, oct_OList args);
//oct_Bool oct_Object_eval(struct oct_Context* ctx, oct_Object obj);
oct_Bool oct_Object_copyOwned(struct oct_Context* ctx, oct_Object obj, void** out_copy);
oct_Bool oct_Object_copyManaged(struct oct_Context* ctx, oct_Object obj, void** out_copy);
oct_Bool oct_Object_hash(struct oct_Context* ctx, oct_Object obj, oct_Uword* out_hash);
oct_Bool oct_Object_equals(struct oct_Context* ctx, oct_Object obj, oct_BObject other, oct_Bool* out_result);

// Some non-vtable helper functions

oct_Bool oct_Object_destroyOwned(struct oct_Context* ctx, oct_OObject obj);

oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);
oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool);

oct_Bool oct_Object_sizeOf(struct oct_Context* ctx, oct_BObject obj, oct_Uword* out_size);

#endif
