#ifndef oct_context
#define oct_context

#include "oct_primitives.h"

struct oct_Runtime;
struct oct_Reader;

typedef struct oct_Context {
	struct oct_Runtime* rt;
	struct oct_Reader* reader;
} oct_Context;

// Private

oct_Bool _oct_Context_initType(oct_Context* ctx);

#endif
