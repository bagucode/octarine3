#ifndef oct_type_pointers
#define oct_type_pointers

#include "oct_primitives.h"
#include "oct_object.h"

struct oct_Type;

typedef struct oct_OType {
    struct oct_Type* ptr;
} oct_OType;

// Borrowed oct_Type
typedef struct oct_BType {
	struct oct_Type* ptr;
} oct_BType;

// Array of Borrowed oct_Type
typedef struct oct_ABType {
	oct_Uword size;
	oct_BType data[];
} oct_ABType;

// Owned array of borrowed Type
typedef struct oct_OABType {
	oct_ABType* ptr;
} oct_OABType;

// Private

struct oct_Context;

oct_Bool _oct_BType_initType(struct oct_Context* ctx);
oct_Bool _oct_ABType_initType(struct oct_Context* ctx);
oct_Bool _oct_OABType_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_OABType_alloc(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result);

oct_Bool oct_Type_asObject(struct oct_Context* ctx, oct_OType type, oct_OObject* out_obj);

#endif

