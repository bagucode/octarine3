#include "oct_vector.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"
#include "oct_object.h"
#include "oct_protocoltype.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Vector_init(struct oct_Context* ctx) {
	oct_CType t;

	// Vector
	t = ctx->rt->builtInTypes.Vector;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Vector);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Vector, size);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_Vector, data);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OAOObjectOption;

	// Object protocol {dtor}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.VectorAsObject, t, oct_Vector_dtor));

	// Printable protocol {print}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Printable, 1, &ctx->rt->vtables.VectorAsPrintable, t, oct_Vector_print));

	// Seq protocol {first,rest,prepend,append,nth}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Seq, 5, &ctx->rt->vtables.VectorAsSeq, t, oct_Vector_first, oct_Vector_rest, oct_Vector_prepend, oct_Vector_append, oct_Vector_nth));

	// BVector
	t = ctx->rt->builtInTypes.BVector;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Vector;

	// OVector
	t = ctx->rt->builtInTypes.OVector;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Vector;

	return oct_True;
}

static oct_Bool construct(struct oct_Context* ctx, oct_Uword cap, oct_BVector self) {
	CHECK(oct_AOObjectOption_createOwned(ctx, cap, &self.ptr->data));
	self.ptr->size = 0;
	return oct_True;
}

oct_Bool oct_Vector_ctor(struct oct_Context* ctx, oct_BVector self) {
	return construct(ctx, 10, self);
}

static oct_Bool create(struct oct_Context* ctx, oct_Uword cap, oct_OVector* out_vec) {
	oct_BVector self;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Vector), (void**)&out_vec->ptr, "oct_Vector_createOwned"));
	self.ptr = out_vec->ptr;
	return construct(ctx, cap, self);
}

oct_Bool oct_Vector_createOwned(struct oct_Context* ctx, oct_OVector* out_vec) {
	return create(ctx, 10, out_vec);
}

oct_Bool oct_Vector_dtor(struct oct_Context* ctx, oct_BVector self) {
	oct_BAOObjectOption baoo;
	oct_Bool result;
	baoo.ptr = self.ptr->data.ptr;
	result = oct_AOObjectOption_dtor(ctx, baoo);
	return OCT_FREEOWNED(self.ptr->data.ptr) && result;
}

oct_Bool oct_Vector_print(struct oct_Context* ctx, oct_BVector self) {
	oct_Uword i;
	oct_BSelf bself;
	oct_BPrintable prn;
	putc('[', stdout);
	for(i = 0; i < self.ptr->size; ++i) {
		if(i > 0) {
			putc(' ', stdout);
		}
		if(self.ptr->data.ptr->data[i].variant == OCT_OOBJECTOPTION_OBJECT) {
			bself.self = self.ptr->data.ptr->data[i].object.self.self;
			CHECK(oct_Object_as(ctx, bself, self.ptr->data.ptr->data[i].object.vtable->type, ctx->rt->builtInProtocols.Printable, (oct_BObject*)&prn));
			CHECK(oct_Printable_print(ctx, prn));
		}
		else {
			puts("Nothing"); // TODO: actually implement Printable for Nothing...
		}
	}
	putc(']', stdout);
	return oct_True;
}

oct_Bool oct_Vector_asObject(struct oct_Context* ctx, oct_BVector self, oct_BObject* out_obj) {
	out_obj->self.self = self.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.VectorAsObject.ptr;
	return oct_True;
}

oct_Bool oct_Vector_asPrintable(struct oct_Context* ctx, oct_BVector self, oct_BPrintable* out_prn) {
	out_prn->self.self = self.ptr;
	out_prn->vtable = (oct_PrintableVTable*)ctx->rt->vtables.VectorAsPrintable.ptr;
	return oct_True;
}

oct_Bool oct_Vecotr_asSeq(struct oct_Context* ctx, oct_BVector self, oct_BSeq* out_seq) {
	out_seq->self.self = self.ptr;
	out_seq->vtable = (oct_SeqVTable*)ctx->rt->vtables.VectorAsSeq.ptr;
	return oct_True;
}

static oct_Bool grow(struct oct_Context* ctx, oct_BVector self) {
	oct_Uword newCap = self.ptr->data.ptr->size * 2;
	oct_OAOObjectOption newArr;
	CHECK(oct_AOObjectOption_createOwned(ctx, newCap, &newArr));
	memcpy(newArr.ptr->data, self.ptr->data.ptr->data, self.ptr->data.ptr->size * sizeof(oct_OObjectOption));
	CHECK(OCT_FREEOWNED(self.ptr->data.ptr));
	self.ptr->data.ptr = newArr.ptr;
	return oct_True;
}

oct_Bool oct_Vector_prepend(struct oct_Context* ctx, oct_BVector self, oct_OObject obj) {
	if(self.ptr->size == self.ptr->data.ptr->size) {
		CHECK(grow(ctx, self));
	}
	memmove(&self.ptr->data.ptr->data[1], &self.ptr->data.ptr->data[0], self.ptr->size * sizeof(oct_OObjectOption));
	self.ptr->data.ptr->data[0].variant = OCT_OOBJECTOPTION_OBJECT;
	self.ptr->data.ptr->data[0].object = obj;
	++self.ptr->size;
	return oct_True;
}

oct_Bool oct_Vector_append(struct oct_Context* ctx, oct_BVector self, oct_OObject obj) {
	if(self.ptr->size == self.ptr->data.ptr->size) {
		CHECK(grow(ctx, self));
	}
	self.ptr->data.ptr->data[self.ptr->size].variant = OCT_OOBJECTOPTION_OBJECT;
	self.ptr->data.ptr->data[self.ptr->size].object = obj;
	++self.ptr->size;
	return oct_True;
}

oct_Bool oct_Vector_first(struct oct_Context* ctx, oct_BVector self, oct_OObjectOption* out_value) {
	if(self.ptr->size == 0) {
		out_value->variant = OCT_OOBJECTOPTION_NOTHING;
	}
	else {
		(*out_value) = self.ptr->data.ptr->data[0];
		self.ptr->data.ptr->data[0].variant = OCT_OOBJECTOPTION_NOTHING;
	}
	return oct_True;
}

oct_Bool oct_Vector_rest(struct oct_Context* ctx, oct_BVector self, oct_OVector* out_vec) {
	oct_Uword i;
	if(self.ptr->size < 2) {
		return oct_Vector_createOwned(ctx, out_vec);
	}
	CHECK(create(ctx, self.ptr->data.ptr->size, out_vec));
	for(i = 1; i < self.ptr->size; ++i) {
		out_vec->ptr->data.ptr->data[i - 1] = self.ptr->data.ptr->data[i];
		self.ptr->data.ptr->data[i].variant = OCT_OOBJECTOPTION_NOTHING;
	}
	out_vec->ptr->size = self.ptr->size - 1;
	return oct_True;
}

oct_Bool oct_Vector_nth(struct oct_Context* ctx, oct_BVector self, oct_Uword idx, oct_OObjectOption* out_value) {
	if(idx > self.ptr->size - 1) {
		out_value->variant = OCT_OOBJECTOPTION_NOTHING;
		return oct_True; // Throw out of bounds error instead? No way to tell the difference from an empty slot now.
	}
	(*out_value) = self.ptr->data.ptr->data[idx];
	self.ptr->data.ptr->data[idx].variant = OCT_OOBJECTOPTION_NOTHING;
	return oct_True;
}

oct_Bool oct_Vector_destroyOwned(struct oct_Context* ctx, oct_OVector vec) {
	oct_BVector bvec;
	oct_Bool result;
	bvec.ptr = vec.ptr;
	result = oct_Vector_dtor(ctx, bvec);
	return OCT_FREEOWNED(vec.ptr) && result;
}

