#ifndef oct_hashtable
#define oct_hashtable

#include "oct_any.h"
#include "oct_hashable.h"
#include "oct_eqcomparable.h"
#include "oct_type_pointers.h"

// Protocol that the keys must satisfy

typedef struct oct_HashtableKeyFunctions {
	oct_HashableFunctions hashable;
	oct_EqComparableFunctions eq;
} oct_HashtableKeyFunctions;

typedef struct oct_HashtableKeyVTable {
	oct_BType type;
	oct_HashtableKeyFunctions functions;
} oct_HashtableKeyVTable;

typedef struct oct_OHashtableKey {
	oct_OSelf self;
	oct_HashtableKeyVTable* vtable;
} oct_OHashtableKey;

typedef struct oct_BHashtableKey {
	oct_BSelf self;
	oct_HashtableKeyVTable* vtable;
} oct_BHashtableKey;

// Hash table

typedef struct oct_HashtableEntry {
	oct_OHashtableKey key;
	oct_Any val;
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

//typedef struct oct_OHashtable {
//	oct_Hashtable* ptr;
//} oct_OHashtable;

typedef struct oct_BHashtable {
	oct_Hashtable* ptr;
} oct_BHashtable;

struct oct_Context;

oct_Bool _oct_Hashtable_init(struct oct_Context* ctx);

oct_Bool oct_AHashtableEntry_createOwned(struct oct_Context* ctx, oct_Uword initialCap, oct_OAHashtableEntry* out_result);
oct_Bool oct_AHashtableEntry_destroyOwned(struct oct_Context* ctx, oct_OAHashtableEntry self);

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap);
oct_Bool oct_Hashtable_dtor(struct oct_Context* ctx, oct_BHashtable self);
oct_Bool oct_Hashtable_put(struct oct_Context* ctx, oct_BHashtable self, oct_OHashtableKey key, oct_Any value);
// Get a value from the table. If the value is owned, it will be removed from the hash table and returned. If the value
// is borrowed, a reference to it will be returned and it will remain in the hash table.
oct_Bool oct_Hashtable_take(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value);
// Get a value from the table. If the value is borrowed, return a reference to it. If the value is owned, return
// a borrowed reference to it. In both cases the value remains in the table.
oct_Bool oct_Hashtable_borrow(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value);

#endif
