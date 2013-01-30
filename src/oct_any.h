#ifndef oct_any_private
#define oct_any_private

#include "oct_primitives.h"
#include "oct_type_pointers.h"
#include "oct_nothing.h"

typedef struct oct_Any {
	oct_Uword data[2];
} oct_Any;

#define OCT_ANYOPTION_NOTHING 0
#define OCT_ANYOPTION_ANY 1

typedef struct oct_AnyOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_Any any;
	};
} oct_AnyOption;

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

oct_Bool oct_Any_getPtrKind(struct oct_Context* ctx, oct_Any any, oct_Uword* out_ptrKind);
oct_Bool oct_Any_getType(struct oct_Context* ctx, oct_Any any, oct_BType* out_type);
oct_Bool oct_Any_getPtr(struct oct_Context* ctx, oct_Any any, void** ptr);

oct_Bool oct_Any_copy(struct oct_Context* ctx, oct_Any any, oct_Any* out_copy);
oct_Bool oct_Any_move(struct oct_Context* ctx, oct_Any release, oct_Any* out_newOwner);

oct_Bool oct_Any_ctor(struct oct_Context* ctx, oct_Any* out_any);
oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_Any any);

#endif
