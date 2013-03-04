#ifndef oct_reader
#define oct_reader

#include "oct_chararray.h"
#include "oct_charstream.h"
#include "oct_object.h"

#define OCT_READRESULT_ERROR 0
#define OCT_READRESULT_OK 1

#define OCT_READERROR_NEED_MORE_DATA 0
#define OCT_READERROR_IO_ERROR 1

typedef struct oct_ReadResult {
	oct_Uword variant;
	union {
		oct_Uword errorCode; // change this to a type that has an error code and an error message string?
		oct_ObjectOption result;
	};
} oct_ReadResult;

typedef struct oct_Reader {
	oct_Uword nchars;
	oct_OAChar readBuffer;
} oct_Reader;

typedef struct oct_BReader {
	oct_Reader* ptr;
} oct_BReader;

// Private

oct_Bool _oct_ReadResult_initType(struct oct_Context* ctx);
oct_Bool _oct_Reader_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_Reader_ctor(struct oct_Context* ctx, oct_Reader* reader);
oct_Bool oct_Reader_read(struct oct_Context* ctx, oct_BReader reader, oct_Charstream input, oct_ReadResult* out_result);

oct_Bool oct_Reader_readFromCString(struct oct_Context* ctx, oct_BReader reader, const char* source, oct_ReadResult* out_result);

oct_Bool oct_ReadResult_dtor(struct oct_Context* ctx, oct_ReadResult* rr);

#endif

