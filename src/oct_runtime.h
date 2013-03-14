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
	oct_BType BHashtable;
	// Type
	oct_BType Type;
	oct_BType BType;
	oct_BType ABType;
	oct_BType OABType;
	// Protocol
	oct_BType Protocol;
	oct_BType VTable;
	oct_BType BVTable;
	oct_BType ProtocolBinding;
	oct_BType BProtocolBinding;
	// Function
	oct_BType Function;
	oct_BType BFunction;
	oct_BType ABFunction;
	oct_BType OABFunction;
	// Object
	oct_BType Object;
	oct_BType OObject;
	oct_BType BObject;
	oct_BType OObjectOption;
	oct_BType BObjectOption;
	// String
	oct_BType String;
	oct_BType OString;
	oct_BType BString;
	// Array
	oct_BType Array;
	oct_BType FixedSizeArray;
	// AChar
	oct_BType AChar;
	oct_BType OAChar;
	// AU8
	oct_BType AU8;
	oct_BType OAU8;
	// List
	oct_BType List;
	oct_BType OList;
	oct_BType BList;
	oct_BType OListOption;
	oct_BType BListOption;
	// Nothing
	oct_BType Nothing;
	oct_BType BNothing;
	// Pointer
	oct_BType Pointer;
	// Struct
	oct_BType Struct;
	// Field
	oct_BType Field;
	oct_BType AField;
	oct_BType OAField;
	// Symbol
	oct_BType Symbol;
	oct_BType OSymbol;
	oct_BType BSymbol;
	oct_BType OSymbolOption;
	// Prototype
	oct_BType Prototype;
	// Variadic
	oct_BType Variadic;
	// Error
	oct_BType Error;
	oct_BType OError;
	oct_BType OErrorOption;
	// Any
	oct_BType Any;
	// Namespace
	oct_BType Namespace;
	oct_BType BNamespace;
	oct_BType NamespaceOption;
} oct_BuiltInTypes;

typedef struct oct_BuiltInVTables {
	// Protocol
	oct_BVTable ProtocolBindingAsObject;
	// String
	oct_BVTable StringAsObject;
	oct_BVTable StringAsEqComparable;
	oct_BVTable StringAsHashable;
	oct_BVTable StringAsHashtableKey;
	// Symbol
	oct_BVTable SymbolAsObject;
	oct_BVTable SymbolAsEqComparable;
	oct_BVTable SymbolAsHashable;
	oct_BVTable SymbolAsHashtableKey;
	// Nothing
	oct_BVTable NothingAsObject;
	oct_BVTable NothingAsEqComparable;
	oct_BVTable NothingAsHashable;
	oct_BVTable NothingAsHashtableKey;
	// List
	oct_BVTable ListAsObject;
} oct_BuiltInVTables;

typedef struct oct_BuiltInProtocols {
	oct_BProtocolBinding Object; // TODO
	oct_BProtocolBinding EqComparable; // TODO
	oct_BProtocolBinding Hashable; // TODO
	oct_BProtocolBinding HashtableKey; // TODO
	oct_BProtocolBinding Copyable; // TODO
} oct_BuiltInProtocols;

typedef struct oct_BuiltInFunctions {
	oct_BFunction hash; // TODO
	oct_BFunction eq; // TODO
} oct_BuiltInFunctions;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	// TODO: lock for namespace collection
	oct_Hashtable namespaces;
	oct_BuiltInTypes builtInTypes;
	oct_BuiltInProtocols builtInProtocols;
	oct_BuiltInVTables vtables;
	oct_BuiltInFunctions functions;
	oct_Nothing nil; // TODO
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
