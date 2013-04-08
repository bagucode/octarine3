#ifndef oct_printable
#define oct_printable

#include "oct_primitives.h"
#include "oct_self.h"
#include "oct_type.h"

struct oct_Context;

typedef struct oct_PrintableFunctions {
    oct_Bool (*print)(struct oct_Context* ctx, oct_BSelf self);
    //oct_Bool (*printToStream)(struct oct_Context* ctx, oct_BSelf self, oct_BCharOutputStream cos);
} oct_PrintableFunctions;

typedef struct oct_PrintableVTable {
	oct_CType type;
	oct_PrintableFunctions functions;
} oct_PrintableVTable;

typedef struct oct_BPrintable {
	oct_BSelf self;
	oct_PrintableVTable* vtable;
} oct_BPrintable;

typedef struct oct_OPrintable {
	oct_OSelf self;
	oct_PrintableVTable* vtable;
} oct_OPrintable;

oct_Bool _oct_Printable_initProtocol(struct oct_Context* ctx);
oct_Bool _oct_Printable_init(struct oct_Context* ctx);

// Helper

oct_Bool oct_Printable_print(struct oct_Context* ctx, oct_BPrintable obj);

#endif
