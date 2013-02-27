#ifndef oct_type_pointers
#define oct_type_pointers

#include "oct_primitives.h"

struct oct_Type;

typedef struct oct_BType {
	struct oct_Type* ptr;
} oct_BType;

oct_BType _oct_BTypeType;

// Array of Borrowed oct_Type
typedef struct oct_ABType {
	oct_Uword size;
	oct_BType data[];
} oct_ABType;

// Owned array of borrowed Type
typedef struct oct_OABType {
	oct_ABType* ptr;
} oct_OABType;

struct oct_Context;

oct_Bool oct_ABType_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result);

struct oct_BHashtableKey;

oct_Bool oct_BType_asHashtableKey(struct oct_Context* ctx, oct_BType self, struct oct_BHashtableKey* key);

//oct_Bool oct_OABType_alloc(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result);
//
//oct_Bool oct_Type_asObject(struct oct_Context* ctx, oct_OType type, oct_OObject* out_obj);
//
//oct_Bool oct_Type_sizeOf(struct oct_Context* ctx, oct_BType type, oct_Uword* out_size);

#endif

