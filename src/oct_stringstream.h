#ifndef oct_stringstream
#define oct_stringstream

#include "oct_string.h"

typedef struct oct_Stringstream {
	oct_Uword idx;
	oct_BString str;
} oct_Stringstream;

typedef struct oct_OStringstream {
	oct_Stringstream* ptr;
} oct_OStringstream;

typedef struct oct_BStringstream {
	oct_Stringstream* ptr;
} oct_BStringstream;

// Private

struct oct_Context;

oct_Bool _oct_Stringstream_init(struct oct_Context* ctx);

// Public

oct_Bool oct_Stringstream_ctor(struct oct_Context* ctx, oct_Stringstream* stream, oct_BString str);
oct_Bool oct_Stringstream_dtor(struct oct_Context* ctx, oct_Stringstream* stream);

oct_Bool oct_OStringstream_create(struct oct_Context* ctx, oct_BString str, oct_OStringstream* out_stream);
oct_Bool oct_OStringstream_destroy(struct oct_Context* ctx, oct_OStringstream stream);

// Charstream protocol
oct_Bool oct_Stringstream_readChar(struct oct_Context* ctx, oct_BStringstream stream, oct_Char* out_char);
oct_Bool oct_Stringstream_peekChar(struct oct_Context* ctx, oct_BStringstream stream, oct_Char* out_char);

struct oct_BCharstream;

oct_Bool oct_Stringstream_asCharStream(struct oct_Context* ctx, oct_BStringstream stream, struct oct_BCharstream* out_cs);

#endif
