#ifndef oct_type
#define oct_type

#include "oct_any.h"
#include "oct_primitives.h"
#include "oct_prototype.h"
#include "oct_templatetype.h"
#include "oct_variadictype.h"
#include "oct_structtype.h"
#include "oct_arraytype.h"
#include "oct_pointertype.h"
#include "oct_interfacetype.h"

#define OCT_TYPE_PROTO 0
#define OCT_TYPE_TEMPLATE 1
#define OCT_TYPE_VARIADIC 2
#define OCT_TYPE_STRUCT 3
#define OCT_TYPE_ARRAY 4
#define OCT_TYPE_FIXED_SIZE_ARRAY 5
#define OCT_TYPE_POINTER 6
#define OCT_TYPE_INTERFACE 7

typedef struct oct_Type {
	oct_Uword variant; // OCT_TYPE_*
	union {
		oct_ProtoType protoType;
		oct_TemplateType templateType;
		oct_VariadicType variadicType;
		oct_StructType structType;
		oct_ArrayType arrayType;
		oct_FixedSizeArrayType fixedSizeoct_ArrayType;
		oct_PointerType pointerType;
		oct_InterfaceType interfaceType;
	};
} oct_Type;

// Private

struct oct_Context;

oct_Bool _oct_Type_initType(struct oct_Context* ctx);

// Public


#endif
