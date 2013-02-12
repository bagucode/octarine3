#include "oct_stringstream.h"
#include "oct_charstream.h"
#include "oct_pointertype.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"

#include <stdlib.h>

// Private

oct_Bool _oct_StringStream_initType(struct oct_Context* ctx) {
}

oct_Bool _oct_OStringStream_initType(struct oct_Context* ctx) {
}

// Public

oct_Bool oct_BStringStream_readChar(struct oct_Context* ctx, oct_BStringStream stream, oct_Char* out_char) {
	if(stream.ptr->idx == stream.ptr->str.ptr->size) {
		*out_char = -1;
		return oct_True;
	}
	// TODO: UTF8. This assumes that a char is a byte
	*out_char = stream.ptr->str.ptr->utf8Data.ptr->data[stream.ptr->idx++];
	return oct_True;
}

oct_Bool oct_BStringStream_peekChar(struct oct_Context* ctx, oct_BStringStream stream, oct_Char* out_char) {
	if(stream.ptr->idx == stream.ptr->str.ptr->size) {
		*out_char = -1;
		return oct_True;
	}
	// TODO: UTF8. This assumes that a char is a byte
	*out_char = stream.ptr->str.ptr->utf8Data.ptr->data[stream.ptr->idx];
	return oct_True;
}

oct_Bool oct_StringStream_ctor(struct oct_Context* ctx, oct_StringStream* stream, oct_BString str) {
	stream->idx = 0;
	stream->str = str;
	return oct_True;
}

oct_Bool oct_StringStream_dtor(struct oct_Context* ctx, oct_StringStream* stream) {
	return oct_True;
}

oct_Bool oct_OStringStream_create(struct oct_Context* ctx, oct_BString str, oct_OStringStream* out_stream) {
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_StringStream), (void**)&out_stream->ptr)) {
        return oct_False;
    }
	return oct_StringStream_ctor(ctx, out_stream->ptr, str);
}

oct_Bool oct_OStringStream_destroy(struct oct_Context* ctx, oct_OStringStream stream) {
    return oct_ExchangeHeap_free(ctx, stream.ptr);
}

oct_Bool oct_BStringStream_asCharStream(struct oct_Context* ctx, oct_BStringStream stream, oct_Charstream* out_cs) {
	oct_CharstreamVTable* vtable;
	// TODO: Do not malloc and construct the vtable here. It is supposed to be stored with the type info somehow...
	vtable = (oct_CharstreamVTable*)malloc(sizeof(oct_CharstreamVTable));
	if(!vtable) {
		return oct_False;
	}
	vtable->instanceType.ptr = ctx->rt->builtInTypes.BStringStream;
	vtable->read = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_readChar;
	vtable->peek = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_peekChar;

	out_cs->object = stream.ptr;
	out_cs->vtable = vtable;

	return oct_True;
}
