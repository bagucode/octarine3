#ifndef oct_charstream
#define oct_charstream

#include "oct_function.h"
#include "oct_any.h"
#include "oct_primitive_pointers.h"

// Charstream interface. Used by the reader

typedef struct oct_CharstreamVTable {
	oct_Bool (*read)(struct oct_Context* ctx, void* b_self /*borrowed*/, oct_Char* out_read);
	oct_Bool (*peek)(struct oct_Context* ctx, void* b_self /*borrowed*/, oct_Char* out_peeked);
} oct_CharstreamVTable;

typedef struct oct_BCharstream {
	void* b_self; // Borrowed
	oct_CharstreamVTable* vtable;
} oct_BCharstream;

// Private

oct_Bool _oct_Charstream_initType(struct oct_Context* ctx);
oct_Bool _oct_CharstreamVTable_initType(struct oct_Context* ctx);

// Public

#endif
