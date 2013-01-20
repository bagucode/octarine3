#include "oct_stringstream.h"
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
	out_stream->ptr = (oct_StringStream*)malloc(sizeof(oct_StringStream));
	if(out_stream->ptr == NULL) {
		// OOM
		return oct_False;
	}
	return oct_StringStream_ctor(ctx, out_stream->ptr, str);
}

oct_Bool oct_OStringStream_destroy(struct oct_Context* ctx, oct_OStringStream stream) {
	free(stream.ptr);
	return oct_True;
}

