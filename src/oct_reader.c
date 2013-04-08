#include "oct_reader.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"
#include "oct_pointertype.h"
#include "oct_runtime.h"
#include "oct_list.h"
#include "oct_stringstream.h"
#include "oct_object.h"
#include "oct_object_vtable.h"
#include "oct_primitives_functions.h"
#include "oct_string.h"

#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <memory.h>
#include <limits.h>

// DEBUG
#include <stdio.h>
// DEBUG

oct_Bool oct_Reader_ctor(struct oct_Context* ctx, oct_Reader* reader) {
	reader->nchars = 0;
	if(!oct_AChar_createOwned(ctx, 200, &reader->readBuffer)) {
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_Reader_dtor(struct oct_Context* ctx, oct_BGeneric self) {
	oct_Reader* r = (oct_Reader*)self.self;
	return OCT_FREEOWNED(r->readBuffer.ptr); // TODO: use destructor
}

oct_Bool oct_ReadResult_dtor(struct oct_Context* ctx, oct_ReadResult* rr) {
	return oct_True;
}

static oct_Bool reader_pushChar(oct_Context* ctx, oct_BReader reader, oct_Char c) {
	oct_OAChar tmp;
	if(reader.ptr->nchars == reader.ptr->readBuffer.ptr->size) {
		if(!oct_AChar_createOwned(ctx, reader.ptr->readBuffer.ptr->size * 2, &tmp)) {
			return oct_False;
		}
		memcpy(tmp.ptr->data, reader.ptr->readBuffer.ptr->data, reader.ptr->nchars * sizeof(oct_Char));
        // TODO: remove this free when OAChar has a proper destructor
		OCT_FREEOWNED(reader.ptr->readBuffer.ptr);
		reader.ptr->readBuffer = tmp;
	}
	reader.ptr->readBuffer.ptr->data[reader.ptr->nchars++] = c;
	return oct_True;
}

static void reader_clearChars(oct_BReader reader) {
	reader.ptr->nchars = 0;
}

static oct_Uword reader_nChars(oct_BReader reader) {
	return reader.ptr->nchars;
}

static oct_Char reader_getChar(oct_BReader reader, oct_Uword idx) {
	return reader.ptr->readBuffer.ptr->data[idx];
}

static oct_Bool stream_peekChar(oct_Context* ctx, oct_BCharstream source, oct_Char* c_out) {
	return source.vtable->functions.peekChar(ctx, source.self, c_out);
}

static oct_Bool stream_discardChar(oct_Context* ctx, oct_BCharstream source) {
	oct_Char c;
	return source.vtable->functions.readChar(ctx, source.self, &c);
}

typedef oct_Bool(*readFn)(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);

static oct_Bool readI32(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readF32(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readString(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readSymbol(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readList(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readVector(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);
static oct_Bool readMap(struct oct_Context*, oct_BReader, oct_BCharstream, oct_ReadResult*);

typedef struct DispatchTableEntry {
	char c;
	readFn fn;
} DispatchTableEntry;

DispatchTableEntry dispatchTable[] = {
	{'0', readI32},    // 1
	{'1', readI32},    // 2
	{'2', readI32},    // 3
	{'3', readI32},    // 4
	{'4', readI32},    // 5
	{'5', readI32},    // 6
	{'6', readI32},    // 7
	{'7', readI32},    // 8
	{'8', readI32},    // 9
	{'9', readI32},    // 10
	{'-', readI32},    // 11
	{'.', readF32},    // 12
	{'(', readList},   // 13
	{'[', readVector}, // 14
	{'{', readMap},    // 15
	{'"', readString}  // 16
};

#define READ_TABLE_SIZE 16

char delims[] = {'"', '(', '[', '{', ')', ']', '}'}; // 7

#define NUM_DELIMS 7

static oct_Bool isdelim(oct_Char c) {
	oct_Uword i;
	if(isspace(c)) {
		return oct_True;
	}
	for(i = 0; i < NUM_DELIMS; ++i) {
		if(c == delims[i]) {
			return oct_True;
		}
	}
	return oct_False;
}

#define CHECK(X) if(!X) goto error
#define PUSH_CHAR(C) CHECK(reader_pushChar(ctx, reader, C))
#define PEEK_CHAR(C) CHECK(stream_peekChar(ctx, source, C))
#define DISCARD_CHAR CHECK(stream_discardChar(ctx, source))
#define NEED_MORE need_more: \
	              out_result->errorCode = OCT_READERROR_NEED_MORE_DATA; \
				  out_result->variant = OCT_READRESULT_ERROR; \
	              goto end

oct_Bool oct_Reader_read(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	oct_Bool result = oct_True;
	oct_Uword i;
	oct_Char next;

	// if we got here from a recursive call we may have data in the reader buffer
	// so we only read in a new token if the reader buffer is empty, otherwise
	// we just do a dispatch
	if(reader_nChars(reader) == 0) {
		// discard whitespace in the stream
		PEEK_CHAR(&next);
		while(isspace(next)) {
			DISCARD_CHAR;
			PEEK_CHAR(&next);
		}
		if(next == -1) { // end of input
			reader_clearChars(reader);
			goto need_more;
		}
		PUSH_CHAR(next);
		DISCARD_CHAR;
		// read a token into the reader buffer
		if(!isdelim(next)) {
			PEEK_CHAR(&next);
			while(next != -1 && !isdelim(next)) {
				PUSH_CHAR(next);
				DISCARD_CHAR;
				PEEK_CHAR(&next);
			}
		}
	}

	// find reader function
	for(i = 0; i < READ_TABLE_SIZE; ++i) {
		if(reader.ptr->readBuffer.ptr->data[0] == dispatchTable[i].c) {
			CHECK(dispatchTable[i].fn(ctx, reader, source, out_result));
			break;
		}
	}
	if(i == READ_TABLE_SIZE) {
		// no dispatch function found, read as symbol
		CHECK(readSymbol(ctx, reader, source, out_result));
	}

	goto end;
	NEED_MORE;
error:
	result = oct_False;
end:
	return result;
}

static oct_Bool readI32(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	char* end;
	long l;
	oct_Uword i = 0;
	char convertBuf[20];
	oct_Bool result = oct_True;
	oct_OI32 instance;

	instance.ptr = NULL;

	if(reader_nChars(reader) > 19) {
		oct_Context_setErrorWithCMessage(ctx, "I32 parse error - too many characters");
		goto error;
	}

	// Copy characters into a char buffer for strtol and check them for errors at the same time
	for(i = 0; i < reader_nChars(reader); ++i) {
		convertBuf[i] = reader_getChar(reader, i);
	    // Allowed chars are minus and numbers.
	    // If the token contains anything else, delegate to float parsing.
		if(convertBuf[i] != '-' && !isdigit(convertBuf[i])) {
			return readF32(ctx, reader, source, out_result);
		}
	}
	convertBuf[i] = 0;

	errno = 0;
	l = strtol(&convertBuf[0], &end, 10);
	if((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
		// Overflow
		goto error;
	}
	if((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
		// Underflow
		goto error;
	}
	if(&convertBuf[0] == end) {
		// Not parseable, try to parse as float
		return readF32(ctx, reader, source, out_result);
	}
	// OK
	out_result->variant = OCT_READRESULT_OK;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_I32), (void**)&instance.ptr, "I32 instance"));
	*instance.ptr = (oct_I32)l;
	
	out_result->result.variant = OCT_OOBJECTOPTION_OBJECT;
	CHECK(oct_I32_asObject(ctx, instance, &out_result->result.object));

	goto end;
error:
	result = oct_False;
	if(instance.ptr) {
		oct_Object_destroyOwned(ctx, out_result->result.object);
	}
end:
	reader_clearChars(reader);
	return result;
}

static oct_Bool readF32(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	char* end;
	double d;
	oct_Uword i;
	char convertBuf[15];
	oct_Bool result = oct_True;
	oct_OF32 instance;

	instance.ptr = NULL;

	if(reader_nChars(reader) > 14) {
		oct_Context_setErrorWithCMessage(ctx, "F32 parse error - too many characters");
		goto error;
	}

	// Copy characters into a char buffer for strtod
	for(i = 0; i < reader_nChars(reader); ++i) {
		convertBuf[i] = reader_getChar(reader, i);
	}
	convertBuf[i] = 0;

	errno = 0;
	d = strtod(&convertBuf[0], &end);
	if((errno == ERANGE && d == HUGE_VAL) || d > FLT_MAX) {
		// Overflow
		return oct_False;
	}
	if((errno == ERANGE && d == -HUGE_VAL) || d < -FLT_MAX) {
		// Underflow
		return oct_False;
	}
	if(&convertBuf[0] == end) {
		// Not parseable, try to parse as symbol
		return readSymbol(ctx, reader, source, out_result);
	}
	// OK
	out_result->variant = OCT_READRESULT_OK;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_F32), (void**)&instance.ptr, "F32 instance"));
	*instance.ptr = (oct_F32)d;

	out_result->result.variant = OCT_OOBJECTOPTION_OBJECT;
	CHECK(oct_F32_asObject(ctx, instance, &out_result->result.object));

	goto end;
error:
	result = oct_False;
	if(instance.ptr) {
		oct_Object_destroyOwned(ctx, out_result->result.object);
	}
end:
	reader_clearChars(reader);
	return result;
}

static oct_Bool readString(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	oct_Bool result = oct_True;
	oct_Char next;
	oct_OString instance;

	instance.ptr = NULL;

	// discard leading "
	reader_clearChars(reader);

	// keep reading chars until we hit the next " because the tokenizer will have stopped at any embedded delimiter
	PEEK_CHAR(&next);
	while(next != -1 && next != '"') {
		// TODO: escape sequences
		// TODO: error if string is not terminated before end of text (NEED MORE)
		PUSH_CHAR(next);
		DISCARD_CHAR;
		PEEK_CHAR(&next);
	}
	// discard ending "
	DISCARD_CHAR;

	out_result->variant = OCT_READRESULT_OK;
	CHECK(oct_String_createOwnedFromCharArray(ctx, reader.ptr->readBuffer, 0, reader.ptr->nchars, &instance));

	out_result->result.variant = OCT_OOBJECTOPTION_OBJECT;
	CHECK(oct_String_asObjectOwned(ctx, instance, &out_result->result.object));

	goto end;
error:
	result = oct_False;
	if(instance.ptr) {
		oct_Object_destroyOwned(ctx, out_result->result.object);
	}
end:
	reader_clearChars(reader);
	return result;
}

static oct_Bool readSymbol(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	oct_Bool result = oct_True;
	oct_OString name;
	oct_OSymbol instance;
	oct_BSymbol bsym;

	name.ptr = NULL;
	instance.ptr = NULL;

	CHECK(oct_String_createOwnedFromCharArray(ctx, reader.ptr->readBuffer, 0, reader.ptr->nchars, &name));
	CHECK(oct_Symbol_createOwned(ctx, name, &instance));

	out_result->variant = OCT_READRESULT_OK;
	out_result->result.variant = OCT_OOBJECTOPTION_OBJECT;

	bsym.ptr = instance.ptr;
	CHECK(oct_Symbol_asObject(ctx, bsym, (oct_BObject*)&out_result->result.object));

	goto end;
error:
	result = oct_False;
	if(instance.ptr) {
		oct_Object_destroyOwned(ctx, out_result->result.object);
	}
	else if(name.ptr) {
		oct_String_destroyOwned(ctx, name);
	}
end:
	reader_clearChars(reader);
	return result;
}

static oct_Bool readList(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	oct_Bool result = oct_True;
	oct_Char next;
	oct_ReadResult content;
	oct_OList olist;
	oct_BList blist;

	CHECK(oct_List_createOwned(ctx, &olist));
	blist.ptr = olist.ptr;

	// Drop leading (
	reader_clearChars(reader);

	// Call read for each token inside the list
	while(oct_True) {
		PEEK_CHAR(&next);
		while(isspace(next)) {
			DISCARD_CHAR;
			PEEK_CHAR(&next);
		}
		if(next == -1 || next == ')') {
			break;
		}
		CHECK(oct_Reader_read(ctx, reader, source, &content));
		if(content.variant == OCT_READRESULT_ERROR) {
			oct_List_destroyOwned(ctx, olist);
			out_result->variant = content.variant;
			out_result->errorCode = content.errorCode;
			goto end;
		}
		CHECK(oct_List_append(ctx, blist, content.result.object));
	}
	// TODO: error if EOF before )
	// discard ending )
	DISCARD_CHAR;

	out_result->variant = OCT_READRESULT_OK;
	out_result->result.variant = OCT_OOBJECTOPTION_OBJECT;
	CHECK(oct_List_asObject(ctx, olist, &out_result->result.object));

	goto end;
error:
	if(olist.ptr) {
		oct_List_destroyOwned(ctx, olist);
	}
	result = oct_False;
end:
	reader_clearChars(reader);
	return result;
}

static oct_Bool readVector(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	reader_clearChars(reader);
	out_result->variant = OCT_READRESULT_OK;
	out_result->result.variant = OCT_OOBJECTOPTION_NOTHING;
	out_result->result.nothing.dummy = 0;
	return oct_True;
}

static oct_Bool readMap(struct oct_Context* ctx, oct_BReader reader, oct_BCharstream source, oct_ReadResult* out_result) {
	reader_clearChars(reader);
	out_result->variant = OCT_READRESULT_OK;
	out_result->result.variant = OCT_OOBJECTOPTION_NOTHING;
	out_result->result.nothing.dummy = 0;
	return oct_True;
}

oct_Bool oct_Reader_readFromCString(struct oct_Context* ctx, oct_BReader reader, const char* source, oct_ReadResult* out_result) {
	oct_OString str;
	oct_BString bstr;
	oct_OStringstream ss;
	oct_BStringstream bss;
	oct_BCharstream stream;
    oct_Bool result = oct_True;
    
    str.ptr = NULL;
    ss.ptr = NULL;
    
    CHECK(oct_String_createOwnedFromCString(ctx, source, &str));
	bstr.ptr = str.ptr;
	CHECK(oct_OStringstream_create(ctx, bstr, &ss));
	bss.ptr = ss.ptr;
	CHECK(oct_Stringstream_asCharStream(ctx, bss, &stream));
	CHECK(oct_Reader_read(ctx, reader, stream, out_result));
    
    goto end;
error:
    result = oct_False;
end:
    if(ss.ptr) {
        oct_OStringstream_destroy(ctx, ss);
    }
    if(str.ptr) {
        oct_String_destroyOwned(ctx, str);
    }
    return result;
}

