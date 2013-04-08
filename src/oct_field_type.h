#ifndef oct_field_type
#define oct_field_type

#include "oct_field.h"
#include "oct_primitive_types.h"
#include "oct_type_type.h"
#include "oct_type.h"

#include <stddef.h>

// Field
oct_AField _oct_FieldFields = {
	2, {
		{offsetof(oct_Field, offset), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_Field, type), {(oct_Type*)&_oct_TypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_FieldType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_Field), {&_oct_FieldFields}}
};

// AField
struct {
	oct_Uword v;
	oct_ArrayType a;
} _oct_AFieldType = {
	OCT_TYPE_ARRAY,
	{{(oct_Type*)&_oct_FieldType}}
};

// CAField definition (declared in type_type)
struct _oct_CAFieldType_t _oct_CAFieldType = {
	OCT_TYPE_POINTER,
	{OCT_POINTER_CONSTANT, {(oct_Type*)&_oct_AFieldType}}
};

#endif
