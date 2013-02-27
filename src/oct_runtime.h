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
	oct_BType U8; // INIT
	oct_BType I8; // INIT
	oct_BType U16; // INIT
	oct_BType I16; // INIT
	oct_BType U32; // INIT
	oct_BType I32; // INIT
	oct_BType U64; // INIT
	oct_BType I64; // INIT
	oct_BType F32; // INIT
	oct_BType F64; // INIT
	oct_BType Uword; // INIT
	oct_BType Word; // INIT
	oct_BType Char; // INIT
	oct_BType Bool; // INIT
	// Hashtable
	oct_BType HashtableKey; // TYPE, INIT
	oct_BType OHashtableKey; // TYPE, INIT
	oct_BType BHashtableKey; // TYPE, INIT
	oct_BType HashtableEntry; // TYPE, INIT
	oct_BType AHashtableEntry; // TYPE, INIT
	oct_BType OAHashtableEntry; // TYPE, INIT
	oct_BType Hashtable; // TYPE, INIT
	oct_BType OHashtable; // TYPE, INIT
	oct_BType BHashtable; // TYPE, INIT
	// Type
	oct_BType Type; // TYPE, INIT
	oct_BType BType; // TYPE, INIT
	// Protocol
	oct_BType Protocol; // INIT
	oct_BType VTable; // INIT
	oct_BType BVTable; // INIT
	oct_BType ProtocolBinding; // INIT
	oct_BType BProtocolBinding; // INIT
	// Function
	oct_BType Function; // TYPE, INIT
	oct_BType BFunction; // TYPE, INIT
	oct_BType ABFunction; // TYPE, INIT
	oct_BType OABFunction; // TYPE, INIT
	// Object
	oct_BType Object; // INIT
	oct_BType OObject; // INIT
	oct_BType BObject; // INIT
	oct_BType OObjectOption; // INIT
	// String
	oct_BType String; // INIT
	oct_BType OString; // INIT
	oct_BType BString; // INIT
	// Array
	oct_BType Array; // INIT
	oct_BType FixedSizeArray; // TYPE, INIT
	// AChar
	oct_BType AChar; // INIT
	oct_BType OAChar; // INIT
	// AU8
	oct_BType AU8; // TYPE, INIT
	oct_BType OAU8; // TYPE, INIT
	// List
	oct_BType List; // INIT
	oct_BType OList; // INIT
	oct_BType BList; // INIT
	oct_BType OListOption; // INIT
	oct_BType BListOption; // INIT
	// Nothing
	oct_BType Nothing; // TYPE, INIT
	// Pointer
	oct_BType Pointer; // INIT
	// Struct
	oct_BType Struct; // INIT
	// Field
	oct_BType Field; // TYPE, INIT
	oct_BType AField; // TYPE, INIT
	oct_BType OAField; // TYPE, INIT
	// Symbol
	oct_BType Symbol; // INIT
	oct_BType OSymbol; // INIT
	oct_BType BSymbol; // INIT
	oct_BType OSymbolOption; // INIT
} oct_BuiltInTypes;

typedef struct oct_BuiltInVTables {
	// Protocol
	oct_BVTable ProtocolBindingAsObject;
	// String
	oct_BVTable StringAsObject; // TODO
	oct_BVTable StringAsEqComparable; // TODO
	oct_BVTable StringAsHashable; // TODO
	oct_BVTable StringAsHashtableKey; // TODO
	// Symbol
	oct_BVTable SymbolAsObject; // TODO
	oct_BVTable SymbolAsEqComparable; // TODO
	oct_BVTable SymbolAsHashable; // TODO
	oct_BVTable SymbolAsHashtableKey; // TODO
} oct_BuiltInVTables;

typedef struct oct_BuiltInProtocols {
	oct_BProtocolBinding Object; // TODO
	oct_BProtocolBinding EqComparable; // TODO
	oct_BProtocolBinding Hashable; // TODO
	oct_BProtocolBinding HashtableKey; // TODO
} oct_BuiltInProtocols;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	// TODO: lock for namespace collection
	oct_Hashtable namespaces;
	oct_BuiltInTypes builtInTypes;
	oct_BuiltInProtocols builtInProtocols;
	oct_BuiltInVTables vtables;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
