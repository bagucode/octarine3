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

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Vector_init(struct oct_Context* ctx) {
	oct_BType t;

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

	// BVector
	t = ctx->rt->builtInTypes.BVector;
	t.ptr->variant == OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Vector;

	// OVector
	t = ctx->rt->builtInTypes.OVector;
	t.ptr->variant == OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Vector;

	return oct_True;
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
