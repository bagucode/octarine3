#ifndef oct_interfacetype
#define oct_interfacetype

#include "oct_function.h"

// The definition of an interface is just a set of function signatures
// for functions that operate primarily on an object of the same type,
// which must be the first argument to each function.

// An instance of an interface is a tuple of pointer to an object and a
// pointer to a virtual call dispatch table that matches the interface
// definition.

// In order to manage memory correctly for the object contained in the
// interface, interfaces are treated like pointers. Ie. there are owned
// managed and borrowed variants of interfaces.
// The pointer kind specification must be compatible with all the functions
// in the interface, eg. if the interface has a function that requires
// the object pointer to be owned then it is an error to declare the interface
// as borrowed or managed.
// The interface may contain functions that take ownership of the object
// pointer. Just like with normal owned pointers, use after ownership transfer
// is checked at the call site of the function taking ownership.

typedef struct oct_InterfaceType {
	oct_Uword ptrkind; // OCT_POINTER_*
	oct_OABFunction functions;
} oct_InterfaceType;

// Private

struct oct_Context;

oct_Bool _oct_Interface_initType(struct oct_Context* ctx);

#endif
