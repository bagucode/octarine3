#ifndef oct_hashtable
#define oct_hashtable

#include "oct_object.h"

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

struct oct_Context;

oct_Bool _oct_Hashtable_init(struct oct_Context* ctx);

#endif
