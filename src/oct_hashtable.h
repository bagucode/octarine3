#ifndef oct_hashtable
#define oct_hashtable

#include "oct_object.h"
#include "oct_hashable.h"
#include "oct_eqcomparable.h"

// Protocol that the keys must satisfy

typedef struct oct_HashtableKeyVTable {
	oct_HashableVTable hashable;
	oct_EqComparableVTable eq;
} oct_HashtableKeyVTable;

typedef struct oct_OHashtableKey {
	oct_OSelf self;
	oct_HashtableKeyVTable vtable;
} oct_OHashtableKey;

typedef struct oct_BHashtableKey {
	oct_BSelf self;
	oct_HashtableKeyVTable vtable;
} oct_BHashtableKey;

// Hash table

typedef struct oct_HashtableEntry {
	oct_OHashtableKey key;
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

struct oct_Context;

oct_Bool _oct_Hashtable_init(struct oct_Context* ctx);

oct_Bool oct_Hashtable_put    (struct oct_Context* ctx, oct_BHashtable self, oct_OHashtableKey key, oct_OObject value);
oct_Bool oct_Hashtable_take   (struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_OObject* out_value);
oct_Bool oct_Hashtable_borrow (struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_BObject* out_value);

#endif
