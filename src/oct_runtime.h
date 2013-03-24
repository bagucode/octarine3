#ifndef oct_runtime
#define oct_runtime

#include "oct_primitives.h"
#include "oct_symbol.h"
#include "oct_hashtable.h"
#include "oct_tls.h"
#include "oct_type_pointers.h"
#include "oct_protocoltype.h"
#include "oct_error.h"

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
	// Self, This is a special type, it is just a marker
	oct_BType BSelf;
	oct_BType OSelf;
	oct_BType MSelf;
	// Hashtable
	oct_BType HashtableKey;
	oct_BType OHashtableKey;
	oct_BType BHashtableKey;
	oct_BType HashtableKeyOption;
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
	oct_BType BAU8;
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
	// Charstream
	oct_BType Charstream;
	oct_BType OCharstream;
	oct_BType BCharstream;
	// Stringstream
	oct_BType Stringstream;
	oct_BType OStringstream;
	oct_BType BStringstream;
	// EqComparable
	oct_BType EqComparable;
	// Hashable
	oct_BType Hashable;
	// Copyable
	oct_BType Copyable;
	// Printable
	oct_BType Printable;
	oct_BType OPrintable;
	oct_BType BPrintable;
} oct_BuiltInTypes;

typedef struct oct_BuiltInVTables {
	// Protocol
	oct_BVTable ProtocolBindingAsObject;
	// String
	oct_BVTable StringAsObject;
	oct_BVTable StringAsEqComparable;
	oct_BVTable StringAsHashable;
	oct_BVTable StringAsHashtableKey;
	oct_BVTable StringAsCopyable;
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
	// Stringstream
	oct_BVTable StringstreamAsCharstream;
	// Type
	oct_BVTable TypeAsObject;
	oct_BVTable TypeAsEqComparable;
	oct_BVTable TypeAsHashable;
	oct_BVTable TypeAsHashtableKey;
	// Primitives
    oct_BVTable U8AsObject;
    oct_BVTable I8AsObject;
    oct_BVTable U16AsObject;
    oct_BVTable I16AsObject;
    oct_BVTable U32AsObject;
    oct_BVTable I32AsObject;
    oct_BVTable I32AsPrintable;
    oct_BVTable U64AsObject;
    oct_BVTable I64AsObject;
    oct_BVTable F32AsObject;
    oct_BVTable F32AsPrintable;
    oct_BVTable F64AsObject;
    oct_BVTable BoolAsObject;
    oct_BVTable CharAsObject;
    oct_BVTable UwordAsObject;
    oct_BVTable WordAsObject;
	// Namespace
	oct_BVTable NamespaceAsObject;
	// VTable
	oct_BVTable VTableAsObject;
} oct_BuiltInVTables;

typedef struct oct_BuiltInProtocols {
	oct_BProtocolBinding Object;
	oct_BProtocolBinding EqComparable;
	oct_BProtocolBinding Hashable;
	oct_BProtocolBinding HashtableKey;
	oct_BProtocolBinding Copyable;
	oct_BProtocolBinding Charstream;
	oct_BProtocolBinding Printable;
} oct_BuiltInProtocols;

typedef struct oct_BuiltInFunctions {
	oct_BFunction hash;
	oct_BFunction eq;
	oct_BFunction readChar;
	oct_BFunction peekChar;
	oct_BFunction copyOwned;
	oct_BFunction dtor;
	oct_BFunction print;
} oct_BuiltInFunctions;

typedef struct oct_BuiltInErrors {
	oct_BError oom;
} oct_BuiltInErrors;

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
	oct_BuiltInErrors errors;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
