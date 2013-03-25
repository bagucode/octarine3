#include "oct_stringstream.h"
#include "oct_charstream.h"
#include "oct_pointertype.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"
#include "oct_type.h"

//#include <stdlib.h>
#include <stddef.h>

// Private

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Stringstream_init(struct oct_Context* ctx) {

	// Stringstream
	oct_BType t = ctx->rt->builtInTypes.Stringstream;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Stringstream);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Stringstream, idx);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Stringstream, str);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.BString;

	// Stringstream vtable for Charstream {readChar, peekChar}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Charstream, 2, &ctx->rt->vtables.StringstreamAsCharstream, t, oct_Stringstream_readChar, oct_Stringstream_peekChar));

	// OStringstream
	t = ctx->rt->builtInTypes.OStringstream;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Stringstream;

	// BStringstream
	t = ctx->rt->builtInTypes.BStringstream;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Stringstream;

	return oct_True;
}

// Public

oct_Bool oct_Stringstream_readChar(struct oct_Context* ctx, oct_BStringstream stream, oct_Char* out_char) {
	if(stream.ptr->idx == stream.ptr->str.ptr->size) {
		*out_char = -1;
		return oct_True;
	}
	// TODO: UTF8. This assumes that a char is a byte
	*out_char = stream.ptr->str.ptr->utf8Data.ptr->data[stream.ptr->idx++];
	return oct_True;
}

oct_Bool oct_Stringstream_peekChar(struct oct_Context* ctx, oct_BStringstream stream, oct_Char* out_char) {
	if(stream.ptr->idx == stream.ptr->str.ptr->size) {
		*out_char = -1;
		return oct_True;
	}
	// TODO: UTF8. This assumes that a char is a byte
	*out_char = stream.ptr->str.ptr->utf8Data.ptr->data[stream.ptr->idx];
	return oct_True;
}

oct_Bool oct_Stringstream_ctor(struct oct_Context* ctx, oct_Stringstream* stream, oct_BString str) {
	stream->idx = 0;
	stream->str = str;
	return oct_True;
}

oct_Bool oct_Stringstream_dtor(struct oct_Context* ctx, oct_Stringstream* stream) {
	return oct_True;
}

oct_Bool oct_OStringstream_create(struct oct_Context* ctx, oct_BString str, oct_OStringstream* out_stream) {
    if(!OCT_ALLOCOWNED(sizeof(oct_Stringstream), (void**)&out_stream->ptr, "oct_OStringstream_create")) {
        return oct_False;
    }
	return oct_Stringstream_ctor(ctx, out_stream->ptr, str);
}

oct_Bool oct_OStringstream_destroy(struct oct_Context* ctx, oct_OStringstream stream) {
    return OCT_FREEOWNED(stream.ptr);
}

oct_Bool oct_Stringstream_asCharStream(struct oct_Context* ctx, oct_BStringstream stream, oct_BCharstream* out_cs) {
	out_cs->self.self = stream.ptr;
	out_cs->vtable = (oct_CharstreamVTable*)ctx->rt->vtables.StringstreamAsCharstream.ptr;
	return oct_True;
}
