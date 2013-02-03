#ifndef oct_interfacetype
#define oct_interfacetype

#include "oct_function.h"

// TODO: change to protocol?
// Besides the name, what needs to be done?
// 

// The definition of an interface is just a set of function signatures
// for functions that operate primarily on an object of the same type,
// which must be the first argument to each function.

// An instance of an interface is a tuple of pointer to an object (dynamically
// typed using an Any instance) and a pointer to a virtual call dispatch table
// that matches the interface definition.

typedef struct oct_InterfaceType {
	oct_OABFunction functions;
} oct_InterfaceType;

// Private

struct oct_Context;

oct_Bool _oct_Interface_initType(struct oct_Context* ctx);

#endif
