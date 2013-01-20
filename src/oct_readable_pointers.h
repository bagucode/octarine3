#ifndef oct_readable_pointers
#define oct_readable_pointers

#include "oct_primitives.h"
#include "oct_nothing.h"

struct oct_Readable;

typedef struct oct_OReadable {
	struct oct_Readable* ptr;
} oct_OReadable;

typedef struct oct_BReadable {
	struct oct_Readable* ptr;
} oct_BReadable;

#define OCT_OREADABLEOPTION_NOTHING 0
#define OCT_OREADABLEOPTION_OREADABLE 1

typedef struct oct_OReadableOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OReadable readable;
	};
} oct_OReadableOption;

// Private

struct oct_Context;

oct_Bool _oct_OReadable_initType(struct oct_Context* ctx);
oct_Bool _oct_BReadable_initType(struct oct_Context* ctx);

oct_Bool _oct_OReadableOption_initType(struct oct_Context* ctx);

#endif
