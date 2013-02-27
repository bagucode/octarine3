#include "oct_primitives.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_field.h"
#include "oct_context.h"

static oct_Bool createPrimitiveType(oct_Context* ctx, oct_BType* t, oct_Uword size, oct_Uword alignment) {
	t->ptr->variant = OCT_TYPE_STRUCT;
	t->ptr->structType.size = size;
	t->ptr->structType.alignment = alignment;
	return oct_AField_createOwned(ctx, 0, &t->ptr->structType.fields);
}

oct_Bool _oct_Primitives_initType(struct oct_Context* ctx) {
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U8, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I8, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U16, 2, 2)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I16, 2, 2)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U32, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I32, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.U64, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.I64, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F32, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.F64, 8, 8)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Bool, 1, 1)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Char, 4, 4)) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Uword, sizeof(oct_Uword), sizeof(oct_Uword))) return oct_False;
	if(!createPrimitiveType(ctx, &ctx->rt->builtInTypes.Word, sizeof(oct_Word), sizeof(oct_Word))) return oct_False;
	return oct_True;
}
