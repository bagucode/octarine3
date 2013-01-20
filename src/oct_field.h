#ifndef oct_field_private
#define oct_field_private

#include "oct_type_pointers.h"

typedef struct oct_Field {
	oct_Uword offset;
	oct_BType type;
} oct_Field;

// Array of field
typedef struct oct_AField {
	oct_Uword size;
	oct_Field data[];
} oct_AField;

// Owned array of field
typedef struct oct_OAField {
	oct_AField* ptr;
} oct_OAField;

// Private

struct oct_Context;

oct_Bool _oct_Field_initType(struct oct_Context* ctx);
oct_Bool _oct_AField_initType(struct oct_Context* ctx);
oct_Bool _oct_OAField_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_Field_ctor(struct oct_Context* ctx, oct_Field* field);

oct_Bool oct_Field_dtor(struct oct_Context* ctx, oct_Field* field);

oct_Bool oct_OAField_alloc(struct oct_Context* ctx, oct_Uword size, oct_OAField* out_result);

oct_Bool oct_OAField_free(struct oct_Context* ctx, oct_OAField oafield);

#endif
