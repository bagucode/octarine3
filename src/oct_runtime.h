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

//typedef struct oct_BuiltInTypes {
//	// Primitives
//	oct_CType U8;
//	oct_CType I8;
//	oct_CType U16;
//	oct_CType I16;
//	oct_CType U32;
//	oct_CType I32;
//	oct_CType U64;
//	oct_CType I64;
//	oct_CType F32;
//	oct_CType F64;
//	oct_CType Uword;
//	oct_CType Word;
//	oct_CType Char;
//	oct_CType Bool;
//	// Self, This is a special type, it is just a marker
//	oct_CType BSelf;
//	oct_CType OSelf;
//	oct_CType MSelf;
//	// Hashtable
//	oct_CType HashtableKey;
//	oct_CType OHashtableKey;
//	oct_CType BHashtableKey;
//	oct_CType HashtableKeyOption;
//	oct_CType HashtableEntry;
//	oct_CType AHashtableEntry;
//	oct_CType OAHashtableEntry;
//	oct_CType Hashtable;
//	oct_CType BHashtable;
//	// Type
//	oct_CType Type;
//	oct_CType BType;
//	oct_CType ABType;
//	oct_CType OABType;
//	// Protocol
//	oct_CType Protocol;
//	oct_CType VTable;
//	oct_CType BVTable;
//	oct_CType ProtocolBinding;
//	oct_CType BProtocolBinding;
//	// Function
//	oct_CType Function;
//	oct_CType CFunction;
//	oct_CType ACFunction;
//	oct_CType OACFunction;
//	// Object
//	oct_CType Object;
//	oct_CType OObject;
//	oct_CType BObject;
//	oct_CType OObjectOption;
//	oct_CType BObjectOption;
//	oct_CType AOObjectOption;
//	oct_CType OAOObjectOption;
//	oct_CType BAOObjectOption;
//	// String
//	oct_CType String;
//	oct_CType OString;
//	oct_CType BString;
//	// Array
//	oct_CType Array;
//	oct_CType FixedSizeArray;
//	// AChar
//	oct_CType AChar;
//	oct_CType OAChar;
//	// AU8
//	oct_CType AU8;
//	oct_CType OAU8;
//	oct_CType BAU8;
//	// List
//	oct_CType List;
//	oct_CType OList;
//	oct_CType BList;
//	oct_CType ListNode;
//	oct_CType OListNode;
//	oct_CType OListNodeOption;
//	// Nothing
//	oct_CType Nothing;
//	oct_CType BNothing;
//	// Pointer
//	oct_CType Pointer;
//	// Struct
//	oct_CType Struct;
//	// Field
//	oct_CType Field;
//	oct_CType AField;
//	oct_CType OAField;
//	// Symbol
//	oct_CType Symbol;
//	oct_CType OSymbol;
//	oct_CType BSymbol;
//	oct_CType OSymbolOption;
//	// Prototype
//	oct_CType Prototype;
//	// Variadic
//	oct_CType Variadic;
//	// Error
//	oct_CType Error;
//	oct_CType OError;
//	oct_CType OErrorOption;
//	// Any
//	oct_CType Any;
//	// Namespace
//	oct_CType Namespace;
//	oct_CType BNamespace;
//	oct_CType NamespaceOption;
//	// Charstream
//	oct_CType Charstream;
//	oct_CType OCharstream;
//	oct_CType BCharstream;
//	// Stringstream
//	oct_CType Stringstream;
//	oct_CType OStringstream;
//	oct_CType BStringstream;
//	// EqComparable
//	oct_CType EqComparable;
//	// Hashable
//	oct_CType Hashable;
//	// Copyable
//	oct_CType Copyable;
//	// Printable
//	oct_CType Printable;
//	oct_CType OPrintable;
//	oct_CType BPrintable;
//	// Vector
//	oct_CType Vector;
//	oct_CType BVector;
//	oct_CType OVector;
//	// Seq
//	oct_CType Seq;
//	oct_CType OSeq;
//	oct_CType BSeq;
//	// Seqable
//	oct_CType Seqable;
//	oct_CType OSeqable;
//	oct_CType BSeqable;
//} oct_BuiltInTypes;
//
//typedef struct oct_BuiltInVTables {
//	// Protocol
//	oct_CVTable ProtocolBindingAsObject;
//	// String
//	oct_CVTable StringAsObject;
//	oct_CVTable StringAsEqComparable;
//	oct_CVTable StringAsHashable;
//	oct_CVTable StringAsHashtableKey;
//	oct_CVTable StringAsCopyable;
//	oct_CVTable StringAsPrintable;
//	// Symbol
//	oct_CVTable SymbolAsObject;
//	oct_CVTable SymbolAsEqComparable;
//	oct_CVTable SymbolAsHashable;
//	oct_CVTable SymbolAsHashtableKey;
//	oct_CVTable SymbolAsPrintable;
//	// Nothing
//	oct_CVTable NothingAsObject;
//	oct_CVTable NothingAsEqComparable;
//	oct_CVTable NothingAsHashable;
//	oct_CVTable NothingAsHashtableKey;
//	// List
//	oct_CVTable ListAsObject;
//	oct_CVTable ListAsPrintable;
//	oct_CVTable ListAsSeq;
//	// Stringstream
//	oct_CVTable StringstreamAsCharstream;
//	// Type
//	oct_CVTable TypeAsObject;
//	oct_CVTable TypeAsEqComparable;
//	oct_CVTable TypeAsHashable;
//	oct_CVTable TypeAsHashtableKey;
//	// Primitives
//    oct_CVTable U8AsObject;
//    oct_CVTable I8AsObject;
//    oct_CVTable U16AsObject;
//    oct_CVTable I16AsObject;
//    oct_CVTable U32AsObject;
//    oct_CVTable I32AsObject;
//    oct_CVTable I32AsPrintable;
//    oct_CVTable U64AsObject;
//    oct_CVTable I64AsObject;
//    oct_CVTable F32AsObject;
//    oct_CVTable F32AsPrintable;
//    oct_CVTable F64AsObject;
//    oct_CVTable BoolAsObject;
//    oct_CVTable CharAsObject;
//    oct_CVTable UwordAsObject;
//    oct_CVTable WordAsObject;
//	// Namespace
//	oct_CVTable NamespaceAsObject;
//	// VTable
//	oct_CVTable VTableAsObject;
//	// AOObjectOption
//	oct_CVTable AOObjectOptionAsObject;
//	// Vector
//	oct_CVTable VectorAsObject;
//	oct_CVTable VectorAsPrintable;
//	oct_CVTable VectorAsSeq;
//} oct_BuiltInVTables;
//
//typedef struct oct_BuiltInProtocols {
//	oct_BProtocolBinding Object;
//	oct_BProtocolBinding EqComparable;
//	oct_BProtocolBinding Hashable;
//	oct_BProtocolBinding HashtableKey;
//	oct_BProtocolBinding Copyable;
//	oct_BProtocolBinding Charstream;
//	oct_BProtocolBinding Printable;
//	oct_BProtocolBinding Seq;
//	oct_BProtocolBinding Seqable;
//} oct_BuiltInProtocols;
//
//typedef struct oct_BuiltInFunctions {
//	oct_CFunction hash;
//	oct_CFunction eq;
//	oct_CFunction readChar;
//	oct_CFunction peekChar;
//	oct_CFunction copyOwned;
//	oct_CFunction dtor;
//	oct_CFunction print;
//	oct_CFunction first;
//	oct_CFunction rest;
//	oct_CFunction prepend;
//	oct_CFunction append;
//	oct_CFunction nth;
//	oct_CFunction seq;
//} oct_BuiltInFunctions;
//
//typedef struct oct_BuiltInErrors {
//	oct_BError oom;
//} oct_BuiltInErrors;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	// TODO: lock for namespace collection
	oct_Hashtable namespaces;
	//oct_BuiltInTypes builtInTypes;
	//oct_BuiltInProtocols builtInProtocols;
	//oct_BuiltInVTables vtables;
	//oct_BuiltInFunctions functions;
	//oct_BuiltInErrors errors;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(oct_Runtime* rt, const char** out_error);

#endif
