#ifndef oct_type_type
#define oct_type_type

#include "oct_field.h"
#include "oct_type.h"
#include "oct_primitive_types.h"

#include <stddef.h>

// Type forward declaration
struct _oct_TypeType_t {
	oct_Uword v;
	oct_VariadicType vt;
} _oct_TypeType;

#define OCT_DEF_POINTER(NAME, KIND, TARGET_TYPE) \
struct { \
	oct_Uword v; \
	oct_PointerType p; \
} _oct_ ## NAME ## Type = { \
	OCT_TYPE_POINTER, \
	{ KIND , {(oct_Type*)&_oct_ ## TARGET_TYPE ## Type}} \
};

#define OCT_DEF_C_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_CONSTANT, TARGET_TYPE)
#define OCT_DEF_B_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_BORROWED, TARGET_TYPE)
#define OCT_DEF_O_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_OWNED, TARGET_TYPE)
#define OCT_DEF_M_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_MANAGED, TARGET_TYPE)
#define OCT_DEF_CP_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_CONSTANT_PROTOCOL, TARGET_TYPE)
#define OCT_DEF_BP_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_BORROWED_PROTOCOL, TARGET_TYPE)
#define OCT_DEF_OP_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_OWNED_PROTOCOL, TARGET_TYPE)
#define OCT_DEF_MP_POINTER(NAME, TARGET_TYPE) OCT_DEF_POINTER(NAME, OCT_POINTER_MANAGED_PROTOCOL, TARGET_TYPE)

#define OCT_DEF_ARRAY(NAME, TARGET_TYPE) \
struct { \
	oct_Uword v; \
	oct_ArrayType a; \
} _oct_ ## NAME ## Type = { \
	OCT_TYPE_ARRAY, \
	{{(oct_Type*)&_oct_ ## TARGET_TYPE ## Type}} \
};

// CType
OCT_DEF_C_POINTER(CType, Type);
// ACType
OCT_DEF_ARRAY(ACType, CType);
// CACType
OCT_DEF_C_POINTER(CACType, ACType);

// ProtoType
oct_AField _oct_ProtoTypeFields = {0};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_ProtoTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_ProtoType), {&_oct_ProtoTypeFields}}
};

// VariadicType
oct_AField _oct_VariadicTypeFields = {
	3, {
		{offsetof(oct_VariadicType, alignment), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_VariadicType, size), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_VariadicType, types), {(oct_Type*)&_oct_CACTypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_VariadicTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_VariadicType), {&_oct_VariadicTypeFields}}
};

// CAField forward declaration
struct _oct_CAFieldType_t {
	oct_Uword v;
	oct_PointerType p;
} _oct_CAFieldType;

// StructType
oct_AField _oct_StructTypeFields = {
	3, {
		{offsetof(oct_StructType, alignment), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_StructType, size), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_StructType, fields), {(oct_Type*)&_oct_CAFieldType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_StructTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_StructType), {&_oct_StructTypeFields}}
};

// ArrayType
oct_AField _oct_ArrayTypeFields = {
	1, {
		{offsetof(oct_ArrayType, elementType), {(oct_Type*)&_oct_TypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_ArrayTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_ArrayType), {&_oct_ArrayTypeFields}}
};

// FixedSizeArrayType
oct_AField _oct_FixedSizeArrayTypeFields = {
	2, {
		{offsetof(oct_FixedSizeArrayType, size), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_FixedSizeArrayType, elementType), {(oct_Type*)&_oct_TypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_FixedSizeArrayTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_FixedSizeArrayType), {&_oct_FixedSizeArrayTypeFields}}
};

// PointerType
oct_AField _oct_PointerTypeFields = {
	2, {
		{offsetof(oct_PointerType, kind), {(oct_Type*)&_oct_UwordType}},
		{offsetof(oct_PointerType, type), {(oct_Type*)&_oct_TypeType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_PointerTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_PointerType), {&_oct_PointerTypeFields}}
};

// CACFunction forward declaration
struct _oct_CACFunctionType_t {
	oct_Uword v;
	oct_PointerType p;
} _oct_CACFunctionType;

// ProtocolType
oct_AField _oct_ProtocolTypeFields = {
	1, {
		{offsetof(oct_ProtocolType, functions), {(oct_Type*)&_oct_CACFunctionType}}
	}
};
struct {
	oct_Uword v;
	oct_StructType s;
} _oct_ProtocolTypeType = {
	OCT_TYPE_STRUCT,
	{0, sizeof(oct_ProtocolType), {&_oct_ProtocolTypeFields}}
};

// TypeType
oct_ACType _oct_TypeTypes = {
	7, {
		{(oct_Type*)&_oct_ProtoTypeType},
		{(oct_Type*)&_oct_VariadicTypeType},
		{(oct_Type*)&_oct_StructTypeType},
		{(oct_Type*)&_oct_ArrayTypeType},
		{(oct_Type*)&_oct_FixedSizeArrayTypeType},
		{(oct_Type*)&_oct_PointerTypeType},
		{(oct_Type*)&_oct_ProtocolTypeType}
	}
};
struct _oct_TypeType_t _oct_TypeType = {
	OCT_TYPE_VARIADIC,
	{ 0, sizeof(oct_Type), {&_oct_TypeTypes}}
};

#endif
