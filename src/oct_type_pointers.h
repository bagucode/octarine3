#ifndef oct_type_pointers
#define oct_type_pointers

#include "oct_primitives.h"

struct oct_Type;

typedef struct oct_CType {
	const struct oct_Type* ptr;
} oct_CType;

// Array of constant oct_Type
typedef struct oct_ACType {
	oct_Uword size;
	oct_CType data[];
} oct_ACType;

// Constant array of constant Type
typedef struct oct_CACType {
	oct_ACType* ptr;
} oct_CACType;

struct oct_Context;

oct_Bool oct_ACType_createOwned(struct oct_Context* ctx, oct_Uword size, oct_CACType* out_result);

struct oct_BHashtableKey;
struct oct_BObject;

oct_Bool oct_Type_equals(struct oct_Context* ctx, oct_CType t1, oct_CType t2, oct_Bool* out_eq);
oct_Bool oct_Type_hash(struct oct_Context* ctx, oct_CType self, oct_Uword* out_hash);

oct_Bool oct_CType_asHashtableKey(struct oct_Context* ctx, oct_CType self, struct oct_BHashtableKey* key);
oct_Bool oct_Type_asObject(struct oct_Context* ctx, oct_CType self, struct oct_BObject* out_obj);

oct_Bool oct_Type_sizeOf(struct oct_Context* ctx, oct_CType type, oct_Uword* out_size);

#endif

