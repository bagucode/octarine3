#ifndef oct_vector
#define oct_vector

#include "oct_object.h"
#include "oct_nothing.h"
#include "oct_printable.h"

typedef struct oct_Vector {
	oct_Uword size;
	oct_OAOObjectOption data;
} oct_Vector;

typedef struct oct_BVector {
	oct_Vector* ptr;
} oct_BVector;

typedef struct oct_OVector {
	oct_Vector* ptr;
} oct_OVector;

struct oct_Context;

oct_Bool _oct_Vector_init(struct oct_Context* ctx);

oct_Bool oct_Vector_dtor(struct oct_Context* ctx, oct_BVector self);
oct_Bool oct_Vector_print(struct oct_Context* ctx, oct_BVector self);

oct_Bool oct_Vector_asObject(struct oct_Context* ctx, oct_BVector self, oct_BObject* out_obj);
oct_Bool oct_Vector_asPrintable(struct oct_Context* ctx, oct_BVector self, oct_BPrintable* out_prn);

#endif
