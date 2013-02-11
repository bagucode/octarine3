#ifndef oct_protocoltype
#define oct_protocoltype

#include "oct_function.h"

// The definition of a protocol is just a set of function signatures
// for functions that operate primarily on an object of the same type,
// which must be the first argument to each function.

// An instance of a protocol, called a protocol object, is a tuple of pointer
// to an object and a pointer to a virtual call dispatch table
// that matches the protocol definition.

typedef struct oct_ProtocolType {
	oct_OABFunction functions;
} oct_ProtocolType;

// Private

struct oct_Context;

oct_Bool _oct_Protocol_initType(struct oct_Context* ctx);

#endif
