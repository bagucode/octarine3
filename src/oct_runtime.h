#ifndef oct_runtime
#define oct_runtime

#include "oct_primitives.h"
#include "oct_symbol.h"
#include "oct_hashtable.h"
#include "oct_tls.h"
#include "oct_type_pointers.h"
#include "oct_protocoltype.h"

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
	oct_BType HashtableKey; // TODO
	oct_BType OHashtableKey; // TODO
	oct_BType BHashtableKey; // TODO
	oct_BType HashtableEntry; // TODO
	oct_BType AHashtableEntry; // TODO
	oct_BType OAHashtableEntry; // TODO
	oct_BType Hashtable; // TODO
	oct_BType OHashtable; // TODO
	oct_BType BHashtable; // TODO
	// Type
	oct_BType Type; // TODO
	oct_BType BType; // TODO
	// Protocol
	oct_BType Protocol;
	oct_BType VTable;
	oct_BType BVTable;
	oct_BType ProtocolBinding;
	oct_BType BProtocolBinding;
	// Function
	oct_BType Function; // TODO
	oct_BType BFunction; // TODO
	oct_BType ABFunction; // TODO
	oct_BType OABFunction; // TODO
	// Object
	oct_BType Object; // TODO
} oct_BuiltInTypes;

typedef struct oct_BuiltInProtocols {
	oct_BProtocolBinding Object;
} oct_BuiltInProtocols;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	// TODO: lock for namespace collection
	oct_Hashtable namespaces;
	oct_BuiltInTypes builtInTypes;
	oct_BuiltInProtocols builtInProtocols;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
