#include "oct_primitives.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_protocoltype.h"

#define CHECK(X) if(!X) return oct_False;

static oct_Bool createPrimitiveType(oct_Context* ctx, oct_BType* t, oct_BVTable* objVtable, oct_Uword size, oct_Uword alignment) {
	t->ptr->variant = OCT_TYPE_STRUCT;
	t->ptr->structType.size = size;
	t->ptr->structType.alignment = alignment;
	CHECK(oct_AField_createOwned(ctx, 0, &t->ptr->structType.fields));
	return _oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 0, objVtable, *t);
}

oct_Bool _oct_Primitives_init(struct oct_Context* ctx) {
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U8, &ctx->rt->vtables.U8AsObject, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I8, &ctx->rt->vtables.I8AsObject, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U16, &ctx->rt->vtables.U16AsObject, 2, 2)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I16, &ctx->rt->vtables.I16AsObject, 2, 2)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U32, &ctx->rt->vtables.U32AsObject, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I32, &ctx->rt->vtables.I32AsObject, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U64, &ctx->rt->vtables.U64AsObject, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I64, &ctx->rt->vtables.I64AsObject, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F32, &ctx->rt->vtables.F32AsObject, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F64, &ctx->rt->vtables.F64AsObject, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Bool, &ctx->rt->vtables.BoolAsObject, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Char, &ctx->rt->vtables.CharAsObject, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Uword, &ctx->rt->vtables.UwordAsObject, sizeof(oct_Uword), sizeof(oct_Uword))) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Word, &ctx->rt->vtables.WordAsObject, sizeof(oct_Word), sizeof(oct_Word))) return oct_False;
	return oct_True;
}
