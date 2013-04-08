#ifndef oct_field_private
#define oct_field_private

#include "oct_type_pointers.h"

typedef struct oct_Field {
	oct_Uword offset;
	oct_CType type;
} oct_Field;

// Array of field
typedef struct oct_AField {
	oct_Uword size;
	oct_Field data[];
} oct_AField;

// Constant array of field
typedef struct oct_CAField {
	oct_AField* ptr;
} oct_CAField;

#endif
