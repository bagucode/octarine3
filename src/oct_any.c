#include "oct_any.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_field.h"
#include "oct_context.h"

oct_Bool _oct_Any_initType(oct_Context* ctx) {
	oct_Type* any = ctx->rt->builtInTypes.Any;
	any->variant = OCT_TYPE_STRUCT;
	any->structType.alignment = sizeof(oct_Uword);
	any->structType.size = sizeof(oct_Any);
	return oct_OAField_alloc(ctx, 0, &any->structType.fields);
}

oct_Bool _oct_AAny_initType(struct oct_Context* ctx) {
	oct_Type* aany = ctx->rt->builtInTypes.AAny;
	aany->variant = OCT_TYPE_ARRAY;
	aany->arrayType.elementType.ptr = ctx->rt->builtInTypes.Any;
	return oct_True;
}

oct_Bool _oct_OAAny_initType(struct oct_Context* ctx) {
	oct_Type* oaany = ctx->rt->builtInTypes.OAAny;
	oaany->variant = OCT_TYPE_POINTER;
	oaany->pointerType.kind = OCT_POINTER_OWNED;
	oaany->pointerType.type.ptr = ctx->rt->builtInTypes.AAny;
	return oct_True;
}

// data[0] holds type pointer and pointer kind
// data[1] holds object pointer

oct_Bool oct_Any_setPtrKind(struct oct_Context* ctx, oct_Any* any, oct_Uword ptrKind) {
	// clear old
	any->data[0] &= (~0x7);
	// set new
	switch(ptrKind) {
	case OCT_POINTER_BORROWED:
		any->data[0] |= 0x1;
		break;
	case OCT_POINTER_MANAGED:
		any->data[0] |= 0x2;
		break;
	case OCT_POINTER_OWNED:
		any->data[0] |= 0x4;
		break;
	}
	return oct_True;
}

oct_Bool oct_Any_setType(struct oct_Context* ctx, oct_Any* any, oct_BType type) {
	// clear old
	any->data[0] &= 0x7;
	// set new
	any->data[0] |= ((oct_Uword)type.ptr);
	return oct_True;
}

oct_Bool oct_Any_setPtr(struct oct_Context* ctx, oct_Any* any, void* ptr) {
	any->data[1] = ((oct_Uword)ptr);
	return oct_True;
}

oct_Bool oct_Any_getPtrKind(struct oct_Context* ctx, oct_Any any, oct_Uword* out_ptrKind) {
	*out_ptrKind = (any.data[0] & 0x7);
	return oct_True;
}

oct_Bool oct_Any_getType(struct oct_Context* ctx, oct_Any any, oct_BType* out_type) {
	out_type->ptr = (oct_Type*)(any.data[0] & (~0x7));
	return oct_True;
}

oct_Bool oct_Any_getPtr(struct oct_Context* ctx, oct_Any any, void** ptr) {
	*ptr = ((void*)any.data[1]);
	return oct_True;
}

oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_Any any) {
	// TODO: implement
}

oct_Bool oct_Any_ctor(struct oct_Context* ctx, oct_Any* out_any) {
	out_any->data[0] = 0;
	out_any->data[1] = 0;
	return oct_True;
}
