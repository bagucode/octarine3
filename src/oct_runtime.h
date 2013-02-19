#ifndef oct_runtime
#define oct_runtime

#include "oct_primitives.h"
#include "oct_symbol.h"
#include "oct_hashtable.h"
#include "oct_tls.h"
#include "oct_type_pointers.h"

struct oct_Context;

typedef struct oct_ContextList {
	struct oct_Context* ctx;
	struct oct_ContextList* next;
} oct_ContextList;

struct oct_Type;

typedef struct oct_BuiltInTypes {
	// Primitives
	oct_BType U8;
	oct_BType I8;
	oct_BType U16;
	oct_BType I16;
	oct_BType U32;
	oct_BType I32;
	oct_BType U64;
	oct_BType I64;
	oct_BType F32;
	oct_BType F64;
	oct_BType Uword;
	oct_BType Word;
	oct_BType Char;
	oct_BType Bool;
	// Hashtable
	oct_BType HashtableKey;
	oct_BType OHashtableKey;
	oct_BType BHashtableKey;
	oct_BType HashtableEntry;
	oct_BType AHashtableEntry;
	oct_BType OAHashtableEntry;
	oct_BType Hashtable;
	oct_BType OHashtable;
	oct_BType BHashtable;
} oct_BuiltInTypes;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	// TODO: lock for namespace collection
	oct_Hashtable namespaces;
	oct_BuiltInTypes builtInTypes;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
