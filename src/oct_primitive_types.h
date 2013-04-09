#ifndef oct_primitive_types
#define oct_primitive_types

#include "oct_field.h"
#include "oct_type.h"

oct_Bool _oct_noopDtor(struct Context* ctx, oct_BGeneric self);

#define OCT_DEF_PRIMITIVE(X) \
oct_AField _oct_ ## X ## Fields = {0}; \
struct { \
	oct_Uword v; \
	oct_StructType s; \
} _oct_ ## X ## Type = { \
	OCT_TYPE_STRUCT, \
	{0, sizeof(oct_ ## X ), {&_oct_ ## X ## Fields}} \
}; \
struct { \
	oct_CType t; \
	void* fns[]; \
} _oct_## X ## ObjectVTable = { \
	(oct_Type*)&_oct_ ## X ## Type, \
	{&_oct_noopDtor} \
}

OCT_DEF_PRIMITIVE(I8);
OCT_DEF_PRIMITIVE(U8);
OCT_DEF_PRIMITIVE(I16);
OCT_DEF_PRIMITIVE(U16);
OCT_DEF_PRIMITIVE(I32);
OCT_DEF_PRIMITIVE(U32);
OCT_DEF_PRIMITIVE(I64);
OCT_DEF_PRIMITIVE(U64);
OCT_DEF_PRIMITIVE(F32);
OCT_DEF_PRIMITIVE(F64);
OCT_DEF_PRIMITIVE(Bool);
OCT_DEF_PRIMITIVE(Char);
OCT_DEF_PRIMITIVE(Word);
OCT_DEF_PRIMITIVE(Uword);

oct_Bool _oct_Primitives_bindTypes(struct oct_Context* ctx);

#endif
