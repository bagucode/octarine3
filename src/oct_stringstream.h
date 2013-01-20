#ifndef oct_stringstream
#define oct_stringstream

#include "oct_string.h"

typedef struct oct_StringStream {
	oct_Uword idx;
	oct_BString str;
} oct_StringStream;

typedef struct oct_OStringStream {
	oct_StringStream* ptr;
} oct_OStringStream;

typedef struct oct_BStringStream {
	oct_StringStream* ptr;
} oct_BStringStream;

// Private

struct oct_Context;

oct_Bool _oct_StringStream_initType(struct oct_Context* ctx);
oct_Bool _oct_OStringStream_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_BStringStream_readChar(struct oct_Context* ctx, oct_BStringStream stream, oct_Char* out_char);
oct_Bool oct_BStringStream_peekChar(struct oct_Context* ctx, oct_BStringStream stream, oct_Char* out_char);

oct_Bool oct_StringStream_ctor(struct oct_Context* ctx, oct_StringStream* stream, oct_BString str);
oct_Bool oct_StringStream_dtor(struct oct_Context* ctx, oct_StringStream* stream);

oct_Bool oct_OStringStream_create(struct oct_Context* ctx, oct_BString str, oct_OStringStream* out_stream);
oct_Bool oct_OStringStream_destroy(struct oct_Context* ctx, oct_OStringStream stream);

#endif
