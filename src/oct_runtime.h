#ifndef oct_runtime
#define oct_runtime

#include "oct_primitives.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_tls.h"

struct oct_Context;

typedef struct oct_ContextList {
	struct oct_Context* ctx;
	struct oct_ContextList* next;
} oct_ContextList;

struct oct_Type;

typedef struct oct_BuiltInTypes {
	// Primitives
	struct oct_Type* U8;
	struct oct_Type* I8;
	struct oct_Type* U16;
	struct oct_Type* I16;
	struct oct_Type* U32;
	struct oct_Type* I32;
	struct oct_Type* U64;
	struct oct_Type* I64;
	struct oct_Type* F32;
	struct oct_Type* F64;
	struct oct_Type* Uword;
	struct oct_Type* Word;
	struct oct_Type* Char;
	struct oct_Type* Bool;
	// Aggregate types
	struct oct_Type* List;
	struct oct_Type* Symbol;
	struct oct_Type* OSymbol;
	struct oct_Type* OSymbolOption;
	struct oct_Type* String;
	struct oct_Type* OString;
	struct oct_Type* Type;
	struct oct_Type* StructType;
	struct oct_Type* ProtoType;
	struct oct_Type* VariadicType;
	struct oct_Type* PointerType;
	struct oct_Type* ArrayType;
	struct oct_Type* FixedSizeArrayType;
	struct oct_Type* Field;
	struct oct_Type* AField;
	struct oct_Type* OAField;
	struct oct_Type* BType;
	struct oct_Type* ABType;
	struct oct_Type* OABType;
	struct oct_Type* AU8;
	struct oct_Type* OAU8;
	struct oct_Type* OList;
	struct oct_Type* MList;
	struct oct_Type* BList;
	struct oct_Type* OListOption;
	struct oct_Type* Reader;
	struct oct_Type* ReadResult;
	struct oct_Type* Nothing;
	struct oct_Type* NamespaceBinding;
	struct oct_Type* ANamespaceBinding;
	struct oct_Type* OANamespaceBinding;
	struct oct_Type* Namespace;
	struct oct_Type* BNamespace;
	struct oct_Type* ProtocolType;
	struct oct_Type* Function;
	struct oct_Type* BFunction;
	struct oct_Type* ABFunction;
	struct oct_Type* OABFunction;
	struct oct_Type* AChar;
	struct oct_Type* OAChar;
	struct oct_Type* BStringStream;
	struct oct_Type* Error;
	struct oct_Type* OError;
	struct oct_Type* ErrorOption;
    struct oct_Type* Object;
    struct oct_Type* OObject;
    struct oct_Type* MObject;
    struct oct_Type* BObject;
    struct oct_Type* OObjectOption;
    struct oct_Type* BObjectOption;
	/* TODO: TYPE HASH! Structurally equal types should be same! */
} oct_BuiltInTypes;

// TODO: Use a hash table instead
typedef struct oct_NamespaceList {
	oct_OSymbol name;
	oct_Namespace ns;
	struct oct_NamespaceList* next;
} oct_NamespaceList;

typedef struct oct_Runtime {
	oct_TLS currentContext;
	// TODO: lock for context collection
	oct_ContextList contextList;
	oct_BuiltInTypes builtInTypes;
	// TODO: lock for namespace collection
	oct_NamespaceList* namespaces;
} oct_Runtime;

struct oct_Context;

oct_Runtime* oct_Runtime_create(const char** out_error);
struct oct_Context* oct_Runtime_currentContext(oct_Runtime* rt);
oct_Bool oct_Runtime_destroy(struct oct_Runtime* rt, const char** out_error);

#endif
