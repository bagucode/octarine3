#ifndef oct_hashtable
#define oct_hashtable

#include "oct_object.h"
#include "oct_list.h"

typedef struct oct_HashtableEntry {
	oct_OObject key;
	oct_OObject val;
} oct_HashtableEntry;

typedef struct oct_AHashtableEntry {
	oct_Uword size;
	oct_HashtableEntry table[];
} oct_AHashtableEntry;

typedef struct oct_OAHashtableEntry {
	oct_AHashtableEntry* ptr;
} oct_OAHashtableEntry;

typedef struct oct_Hashtable {
	oct_OAHashtableEntry table;
} oct_Hashtable;

typedef struct oct_OHashtable {
	oct_Hashtable* ptr;
} oct_OHashtable;

typedef struct oct_BHashtable {
	oct_Hashtable* ptr;
} oct_BHashtable;

typedef struct oct_MHashtable {
	oct_Hashtable* ptr;
} oct_MHashtable;

struct oct_Context;

oct_Bool _oct_Hashtable_init(struct oct_Context* ctx);

// Object protocol
oct_Bool oct_Hashtable_dtor        (struct oct_Context* ctx, oct_Hashtable* self);
oct_Bool oct_Hashtable_invoke      (struct oct_Context* ctx, oct_BHashtable self, oct_OList args);
oct_Bool oct_Hashtable_copyOwned   (struct oct_Context* ctx, oct_BHashtable self, oct_OHashtable* out_copy);
oct_Bool oct_Hashtable_copyManaged (struct oct_Context* ctx, oct_BHashtable self, oct_MHashtable* out_copy);
oct_Bool oct_Hashtable_hash        (struct oct_Context* ctx, oct_BHashtable self, oct_Uword* out_hash);
oct_Bool oct_Hashtable_equals      (struct oct_Context* ctx, oct_BHashtable self, oct_BObject other, oct_Bool* out_result);

// Hashtable specific
oct_Bool oct_Hashtable_put    (struct oct_Context* ctx, oct_BHashtable self, oct_OObject key, oct_OObject value);
oct_Bool oct_Hashtable_take   (struct oct_Context* ctx, oct_BHashtable self, oct_BObject key, oct_OObject* out_value);
oct_Bool oct_Hashtable_borrow (struct oct_Context* ctx, oct_BHashtable self, oct_BObject key, oct_BObject* out_value);

#endif
