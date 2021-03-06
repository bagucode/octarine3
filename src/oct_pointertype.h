#ifndef oct_pointertype_private
#define oct_pointertype_private

#include "oct_primitives.h"
#include "oct_type_pointers.h"

#define OCT_POINTER_BORROWED 0
#define OCT_POINTER_OWNED 1
#define OCT_POINTER_MANAGED 2
#define OCT_POINTER_CONSTANT 3

#define OCT_POINTER_BORROWED_PROTOCOL 4
#define OCT_POINTER_OWNED_PROTOCOL 5
#define OCT_POINTER_MANAGED_PROTOCOL 6
#define OCT_POINTER_CONSTANT_PROTOCOL 7

typedef struct oct_PointerType {
	oct_Uword kind; // OCT_POINTER_*
	oct_CType type;
} oct_PointerType;

struct oct_Context;

oct_Bool _oct_PointerType_init(struct oct_Context* ctx);

#endif
