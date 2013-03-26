#ifndef oct_vector
#define oct_vector

#include "oct_object.h"
#include "oct_nothing.h"
#include "oct_printable.h"
#include "oct_seq.h"

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

oct_Bool oct_Vector_ctor(struct oct_Context* ctx, oct_BVector self);

oct_Bool oct_Vector_createOwned(struct oct_Context* ctx, oct_OVector* out_vec);
oct_Bool oct_Vector_destroyOwned(struct oct_Context* ctx, oct_OVector vec);

oct_Bool oct_Vector_dtor(struct oct_Context* ctx, oct_BVector self);
oct_Bool oct_Vector_print(struct oct_Context* ctx, oct_BVector self);
oct_Bool oct_Vector_prepend(struct oct_Context* ctx, oct_BVector self, oct_OObject obj);
oct_Bool oct_Vector_append(struct oct_Context* ctx, oct_BVector self, oct_OObject obj);
oct_Bool oct_Vector_first(struct oct_Context* ctx, oct_BVector self, oct_OObjectOption* out_value);
oct_Bool oct_Vector_rest(struct oct_Context* ctx, oct_BVector self, oct_OVector* out_vec);
oct_Bool oct_Vector_nth(struct oct_Context* ctx, oct_BVector self, oct_Uword idx, oct_OObjectOption* out_value);

oct_Bool oct_Vector_asObject(struct oct_Context* ctx, oct_BVector self, oct_BObject* out_obj);
oct_Bool oct_Vector_asPrintable(struct oct_Context* ctx, oct_BVector self, oct_BPrintable* out_prn);
oct_Bool oct_Vecotr_asSeq(struct oct_Context* ctx, oct_BVector self, oct_BSeq* out_seq);

#endif
