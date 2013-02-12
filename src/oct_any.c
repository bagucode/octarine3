//#include "oct_object.h"
//#include "oct_runtime.h"
//#include "oct_type.h"
//#include "oct_field.h"
//#include "oct_context.h"
//
//oct_Bool _oct_Any_initType(oct_Context* ctx) {
//	oct_Type* any = ctx->rt->builtInTypes.Any;
//	any->variant = OCT_TYPE_STRUCT;
//	any->structType.alignment = sizeof(oct_Uword);
//	any->structType.size = sizeof(oct_Any);
//	return oct_OAField_alloc(ctx, 0, &any->structType.fields);
//}
//
//oct_Bool _oct_AAny_initType(struct oct_Context* ctx) {
//	oct_Type* aany = ctx->rt->builtInTypes.AAny;
//	aany->variant = OCT_TYPE_ARRAY;
//	aany->arrayType.elementType.ptr = ctx->rt->builtInTypes.Any;
//	return oct_True;
//}
//
//oct_Bool _oct_OAAny_initType(struct oct_Context* ctx) {
//	oct_Type* oaany = ctx->rt->builtInTypes.OAAny;
//	oaany->variant = OCT_TYPE_POINTER;
//	oaany->pointerType.kind = OCT_POINTER_OWNED;
//	oaany->pointerType.type.ptr = ctx->rt->builtInTypes.AAny;
//	return oct_True;
//}
//
//oct_Bool _OCT_OOBJECTOPTION_initType(struct oct_Context* ctx) {
//	oct_Type* t = ctx->rt->builtInTypes.AnyOption;
//	t->variant = OCT_TYPE_VARIADIC;
//	t->variadicType.alignment = 0;
//	t->variadicType.size = sizeof(oct_OObjectOption);
//	if(!oct_OABType_alloc(ctx, 2, &t->variadicType.types)) {
//		return oct_False;
//	}
//	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.Nothing;
//	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.Any;
//	return oct_True;
//}
//
//// data[0] holds type pointer and pointer kind
//// data[1] holds object pointer
//
//oct_Bool oct_Any_setPtrKind(struct oct_Context* ctx, oct_Any* any, oct_Uword ptrKind) {
//	// clear old
//	any->data[0] &= (~0x7);
//	// set new
//	any->data[0] |= ptrKind;
//	return oct_True;
//}
//
//oct_Bool oct_Any_setType(struct oct_Context* ctx, oct_Any* any, oct_BType type) {
//	// clear old
//	any->data[0] &= 0x7;
//	// set new
//	any->data[0] |= ((oct_Uword)type.ptr);
//	return oct_True;
//}
//
//oct_Bool oct_Any_setPtr(struct oct_Context* ctx, oct_Any* any, void* ptr) {
//	any->data[1] = ((oct_Uword)ptr);
//	return oct_True;
//}
//
//oct_Bool oct_Any_setAll(struct oct_Context* ctx, oct_Any* any, oct_Uword ptrKind, oct_BType type, void* ptr) {
//	oct_Any_setPtrKind(ctx, any, ptrKind);
//	oct_Any_setPtr(ctx, any, ptr);
//	oct_Any_setType(ctx, any, type);
//	return oct_True;
//}
//
//oct_Bool oct_Any_getPtrKind(struct oct_Context* ctx, oct_Any any, oct_Uword* out_ptrKind) {
//	*out_ptrKind = (any.data[0] & 0x7);
//	return oct_True;
//}
//
//oct_Bool oct_Any_getType(struct oct_Context* ctx, oct_Any any, oct_BType* out_type) {
//	out_type->ptr = (oct_Type*)(any.data[0] & (~0x7));
//	return oct_True;
//}
//
//oct_Bool oct_Any_getPtr(struct oct_Context* ctx, oct_Any any, void** ptr) {
//	*ptr = ((void*)any.data[1]);
//	return oct_True;
//}
//
//oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_Any any) {
//	// TODO: implement
//	return oct_True;
//}
//
//oct_Bool oct_Any_ctor(struct oct_Context* ctx, oct_Any* out_any) {
//	out_any->data[0] = 0;
//	out_any->data[1] = 0;
//	return oct_True;
//}
//
//oct_Bool oct_Any_symbolp(struct oct_Context* ctx, oct_Any any, oct_Bool* out_bool) {
//	oct_BType t;
//    oct_Any_getType(ctx, any, &t);
//	*out_bool = ctx->rt->builtInTypes.Symbol == t.ptr;
//    return oct_True;
//}
//
//oct_Bool oct_Any_stringp(struct oct_Context* ctx, oct_Any any, oct_Bool* out_bool) {
//	oct_BType t;
//    oct_Any_getType(ctx, any, &t);
//	*out_bool = ctx->rt->builtInTypes.String == t.ptr;
//    return oct_True;
//}
//
//oct_Bool oct_Any_listp(struct oct_Context* ctx, oct_Any any, oct_Bool* out_bool) {
//	oct_BType t;
//    oct_Any_getType(ctx, any, &t);
//	*out_bool = ctx->rt->builtInTypes.List == t.ptr;
//    return oct_True;
//}
//
//oct_Bool oct_Any_copy(struct oct_Context* ctx, oct_Any any, oct_Any* out_copy) {
//	// TODO: check if type is copyable
//	// TODO: implement deep copy of types
//    return oct_True;
//}
//
//oct_Bool oct_Any_move(struct oct_Context* ctx, oct_Any release, oct_Any* out_newOwner) {
//	// no logic needed here, for the C code all we can do is a simple assignment and
//	// then trust the user not to use the released variable after this operation
//	out_newOwner->data[0] = release.data[0];
//	out_newOwner->data[1] = release.data[1];
//	return oct_True;
//}
//
//// Assign does a simple assignment or a move based on the default action for the contained pointer type
//oct_Bool oct_Any_assign(struct oct_Context* ctx, oct_Any any, oct_Any* out_any) {
//	// Since the implementation of move is just an assignment, delegate to it
//	return oct_Any_move(ctx, any, out_any);
//}
//
//oct_Bool oct_Any_borrow(struct oct_Context* ctx, oct_Any any, oct_Any* out_borrowed) {
//	// Nothing we can check here. In the C code we just have to trust the user
//	out_borrowed->data[0] = any.data[0];
//	out_borrowed->data[1] = any.data[1];
//	return oct_True;
//}
