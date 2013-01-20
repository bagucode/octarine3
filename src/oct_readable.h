#ifndef oct_readable_private
#define oct_readable_private

// Variadic type that can be any of the readable types:
// I32 (default, others should be readable with extra syntax)
// F32 (default, others should be readable with extra syntax)
// oct_String
// oct_Symbol
// List<Readable>
// Vector<Readable>
// HashMap<Readable,Readable>

#include "oct_list.h"
#include "oct_string.h"
#include "oct_symbol.h"

#define OCT_READABLE_INT 1
#define OCT_READABLE_FLOAT 2
#define OCT_READABLE_STRING 3
#define OCT_READABLE_SYMBOL 4
#define OCT_READABLE_LIST 5
#define OCT_READABLE_VECTOR 6
#define OCT_READABLE_MAP 7

typedef struct oct_Readable {
	oct_Uword variant; // OCT_READABLE_*
	union {
		oct_I32 i32;
		oct_F32 f32;
		oct_String string;
		oct_Symbol symbol;
		oct_ReadableList list;
		// oct_ReadableVector vec;
		// oct_ReadableMap map;
	};
} oct_Readable;

struct oct_Context;

oct_Bool _oct_Readable_initType(struct oct_Context* ctx);

#endif
