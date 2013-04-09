#include "oct_primitives.h"
#include "oct_primitives_functions.h"
#include "oct_primitive_types.h"
#include "oct_primitive_pointers.h"
#include "oct_object_type.h"

#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_protocoltype.h"
#include "oct_namespace.h"

//#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_noopDtor(struct Context* ctx, oct_BGeneric self) {
	return oct_True;
}

//static oct_Bool createPrimitiveType(oct_Context* ctx, oct_CType* t, oct_CVTable* objVtable, oct_Uword size, oct_Uword alignment) {
//	t->ptr->variant = OCT_TYPE_STRUCT;
//	t->ptr->structType.size = size;
//	t->ptr->structType.alignment = alignment;
//	CHECK(oct_AField_createOwned(ctx, 0, &t->ptr->structType.fields));
//	return _oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, objVtable, *t, noopDtor);
//}
//
//oct_Bool _oct_Primitives_init(struct oct_Context* ctx) {
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U8, &ctx->rt->vtables.U8AsObject, 1, 1)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I8, &ctx->rt->vtables.I8AsObject, 1, 1)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U16, &ctx->rt->vtables.U16AsObject, 2, 2)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I16, &ctx->rt->vtables.I16AsObject, 2, 2)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U32, &ctx->rt->vtables.U32AsObject, 4, 4)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I32, &ctx->rt->vtables.I32AsObject, 4, 4)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U64, &ctx->rt->vtables.U64AsObject, 8, 8)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I64, &ctx->rt->vtables.I64AsObject, 8, 8)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F32, &ctx->rt->vtables.F32AsObject, 4, 4)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F64, &ctx->rt->vtables.F64AsObject, 8, 8)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Bool, &ctx->rt->vtables.BoolAsObject, 1, 1)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Char, &ctx->rt->vtables.CharAsObject, 4, 4)) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Uword, &ctx->rt->vtables.UwordAsObject, sizeof(oct_Uword), sizeof(oct_Uword))) return oct_False;
//	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Word, &ctx->rt->vtables.WordAsObject, sizeof(oct_Word), sizeof(oct_Word))) return oct_False;
//	return oct_True;
//}

#define REG(X) result = result && _oct_Runtime_bindBuiltInType(ctx, #X, &_oct_##X##Type); \
	ct.ptr = (oct_Type*)&_oct_##X##Type; \
	cvt.ptr = (oct_VTable*)&_oct_##X##ObjectVTable; \
	result = result && oct_Protocol_addImplementation(ctx, pb, ct, cvt)

oct_Bool _oct_Primitives_bindTypes(oct_Context* ctx) {
	oct_Bool result = oct_True;
	oct_BProtocolBinding pb;
	oct_CType ct;
	oct_CVTable cvt;
	pb.ptr = &_oct_ObjectProtocol;
	REG(U8);
	REG(I8);
	REG(U16);
	REG(I16);
	REG(U32);
	REG(I32);
	REG(U64);
	REG(I64);
	REG(F32);
	REG(F64);
	REG(Bool);
	REG(Char);
	REG(Word);
	REG(Uword);
	return result;
}
