#ifndef oct_any_private
#define oct_any_private

#include "oct_primitives.h"
#include "oct_type_pointers.h"

typedef struct oct_Any {
	oct_Uword data[2];
} oct_Any;

typedef struct oct_AAny {
	oct_Uword size;
	oct_Any data[];
} oct_AAny;

typedef struct oct_OAAny {
	oct_AAny* ptr;
} oct_OAAny;

// Private

struct oct_Context;

oct_Bool _oct_Any_initType(struct oct_Context* ctx);
oct_Bool _oct_AAny_initType(struct oct_Context* ctx);
oct_Bool _oct_OAAny_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_Any_setPtrKind(struct oct_Context* ctx, oct_Any* any, oct_Uword ptrKind);
oct_Bool oct_Any_setType(struct oct_Context* ctx, oct_Any* any, oct_BType type);
oct_Bool oct_Any_setPtr(struct oct_Context* ctx, oct_Any* any, void* ptr);

#endif
