#ifndef oct_charstream
#define oct_charstream

#include "oct_function.h"
#include "oct_object.h"
#include "oct_primitive_pointers.h"
#include "oct_type_pointers.h"
#include "oct_self.h"

// Charstream protocol. Used by the reader

typedef struct oct_CharstreamFunctions {
	oct_Bool (*readChar)(struct oct_Context* ctx, oct_BSelf self, oct_Char* out_read);
	oct_Bool (*peekChar)(struct oct_Context* ctx, oct_BSelf self, oct_Char* out_peeked);
} oct_CharstreamFunctions;

typedef struct oct_CharstreamVTable {
	oct_BType type;
	oct_CharstreamFunctions functions;
} oct_CharstreamVTable;

typedef struct oct_BCharstream {
	oct_BSelf self;
	oct_CharstreamVTable* vtable;
} oct_BCharstream;

typedef struct oct_OCharstream {
	oct_OSelf self;
	oct_CharstreamVTable* vtable;
} oct_OCharstream;

oct_Bool _oct_Charstream_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Charstream_init(struct oct_Context* ctx);

#endif
