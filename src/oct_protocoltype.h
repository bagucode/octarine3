#ifndef oct_protocoltype
#define oct_protocoltype

#include "oct_function.h"
#include "oct_hashtable.h"

// The definition of a protocol is just a set of function signatures
// for functions that operate primarily on an object of the same type,
// which must be the first argument to each function.

// An instance of a protocol, called a protocol object, is a tuple of pointer
// to an object and a pointer to a virtual call dispatch table
// that matches the protocol definition.

typedef struct oct_ProtocolType {
	oct_OABFunction functions;
} oct_ProtocolType;

typedef struct oct_VTable {
	oct_BType objectType;
	void* functions[]; /* zero or more function pointers */
} oct_VTable;

typedef struct oct_BVTable {
	oct_VTable* ptr;
} oct_BVTable;

// This type is what is put in a namespace when creating or linking a new protocol.
// It contains the protocol definition and a table of implementations for all implementing types.
typedef struct oct_ProtocolBinding {
	oct_BType protocolType;
	oct_Hashtable implementations;
} oct_ProtocolBinding;

// Only borrowed pointer with global scope. These are never deleted.
typedef struct oct_BProtocolBinding {
	oct_ProtocolBinding* ptr;
} oct_BProtocolBinding;

struct oct_Context;

oct_Bool _oct_Protocol_init(struct oct_Context* ctx);
oct_Bool _oct_Protocol_addBuiltIn(struct oct_Context* ctx, oct_BProtocolBinding pb, oct_Uword fnCount, oct_BVTable* table, oct_BType type, ...);

oct_Bool oct_Protocol_addImplementation(struct oct_Context* ctx, oct_BProtocolBinding protocol, oct_BType type, oct_BVTable vtable);

#endif
