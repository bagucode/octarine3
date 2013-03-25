#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

// Private

#define CHECK(X) if(!X) return oct_False;

// For Type, the vtables need to be added manually because of a circular dependency with protocols
oct_Bool _oct_Type_VTableInit(struct oct_Context* ctx) {
	// Object
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.TypeAsObject.ptr, "_oct_Type_VTableInit, Object"));
	ctx->rt->vtables.TypeAsObject.ptr->functions[0] = oct_Type_dtor;
	ctx->rt->vtables.TypeAsObject.ptr->objectType = ctx->rt->builtInTypes.Type;

	// EqComparable
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.TypeAsEqComparable.ptr, "_oct_Type_VTableInit, EqComparable"));
	ctx->rt->vtables.TypeAsEqComparable.ptr->functions[0] = oct_Type_equals;
	ctx->rt->vtables.TypeAsEqComparable.ptr->objectType = ctx->rt->builtInTypes.Type;
	
	// Hashable
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 1), (void**)&ctx->rt->vtables.TypeAsHashable.ptr, "_oct_Type_VTableInit, Hashable"));
	ctx->rt->vtables.TypeAsHashable.ptr->functions[0] = oct_Type_hash;
	ctx->rt->vtables.TypeAsHashable.ptr->objectType = ctx->rt->builtInTypes.Type;
	
	// HashtableKey
	CHECK(OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 2), (void**)&ctx->rt->vtables.TypeAsHashtableKey.ptr, "_oct_Type_VTableInit, HashtableKey"));
	ctx->rt->vtables.TypeAsHashtableKey.ptr->functions[0] = oct_Type_hash;
	ctx->rt->vtables.TypeAsHashtableKey.ptr->functions[1] = oct_Type_equals;
	ctx->rt->vtables.TypeAsHashtableKey.ptr->objectType = ctx->rt->builtInTypes.Type;

	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Object, ctx->rt->builtInTypes.Type, ctx->rt->vtables.TypeAsObject);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.EqComparable, ctx->rt->builtInTypes.Type, ctx->rt->vtables.TypeAsEqComparable);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Hashable, ctx->rt->builtInTypes.Type, ctx->rt->vtables.TypeAsHashable);
	oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.HashtableKey, ctx->rt->builtInTypes.Type, ctx->rt->vtables.TypeAsHashtableKey);

	return oct_True;
}

oct_Bool _oct_Type_init(oct_Context* ctx) {

	// Type
	oct_BType t = ctx->rt->builtInTypes.Type;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_Type);
	CHECK(oct_ABType_createOwned(ctx, 7, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Prototype;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.Variadic;
	t.ptr->variadicType.types.ptr->data[2] = ctx->rt->builtInTypes.Struct;
	t.ptr->variadicType.types.ptr->data[3] = ctx->rt->builtInTypes.Array;
	t.ptr->variadicType.types.ptr->data[4] = ctx->rt->builtInTypes.FixedSizeArray;
	t.ptr->variadicType.types.ptr->data[5] = ctx->rt->builtInTypes.Pointer;
	t.ptr->variadicType.types.ptr->data[6] = ctx->rt->builtInTypes.Protocol;

	// BType
	ctx->rt->builtInTypes.BType.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BType.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BType.ptr->pointerType.type = ctx->rt->builtInTypes.Type;

	// ABType
	ctx->rt->builtInTypes.ABType.ptr->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ABType.ptr->arrayType.elementType = ctx->rt->builtInTypes.BType;

	// OABType
	ctx->rt->builtInTypes.OABType.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OABType.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OABType.ptr->pointerType.type = ctx->rt->builtInTypes.ABType;

	return oct_True;
}

// Public

oct_Bool oct_ABType_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result) {
	oct_Uword i;
    if(!OCT_ALLOCOWNED(sizeof(oct_ABType) + (sizeof(oct_BType) * size), (void**)&out_result->ptr, "oct_ABType_createOwned")) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// Initialize all to Nothing
	for(i = 0; i < size; ++i) {
		out_result->ptr->data[i] = ctx->rt->builtInTypes.Nothing;
	}
	return oct_True;
}

oct_Bool oct_Type_sizeOf(struct oct_Context* ctx, oct_BType type, oct_Uword* out_size) {
	oct_Uword elementSize;
	switch(type.ptr->variant) {
	case OCT_TYPE_PROTOTYPE:
		*out_size = sizeof(void*);
		break;
	case OCT_TYPE_PROTOCOL:
		*out_size = sizeof(void*) * 2;
		break;
	case OCT_TYPE_POINTER:
		*out_size = sizeof(void*);
		break;
	case OCT_TYPE_VARIADIC:
		*out_size = type.ptr->variadicType.size;
		break;
	case OCT_TYPE_STRUCT:
		*out_size = type.ptr->structType.size;
		break;
	case OCT_TYPE_FIXED_SIZE_ARRAY:
		oct_Type_sizeOf(ctx, type.ptr->fixedSizeArray.elementType, &elementSize);
		*out_size = type.ptr->fixedSizeArray.size * elementSize;
		break;
	case OCT_TYPE_ARRAY:
		*out_size = 0; // incorrect, but the actual size is unknown without looking at the instance
		break;
	}
	return oct_True;
}

oct_Bool oct_Type_asObject(struct oct_Context* ctx, oct_BType self, struct oct_BObject* out_obj) {
	out_obj->self.self = self.ptr;
	out_obj->vtable = (oct_ObjectVTable*)ctx->rt->vtables.TypeAsObject.ptr;
	return oct_True;
}

oct_Bool oct_BType_asHashtableKey(struct oct_Context* ctx, oct_BType self, struct oct_BHashtableKey* key) {
	key->self.self = self.ptr;
	key->vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.TypeAsHashtableKey.ptr;
	return oct_True;
}

oct_Bool oct_Type_equals(struct oct_Context* ctx, oct_BType t1, oct_BType t2, oct_Bool* out_eq) {
	*out_eq = t1.ptr == t2.ptr;
	return oct_True;
}

oct_Bool oct_Type_hash(struct oct_Context* ctx, oct_BType self, oct_Uword* out_hash) {
	*out_hash = (oct_Uword)self.ptr;
	return oct_True;
}

oct_Bool oct_Type_dtor(struct oct_Context* ctx, oct_Type* self) {
	switch(self->variant) {
	case OCT_TYPE_PROTOTYPE:
		break;
	case OCT_TYPE_VARIADIC:
		OCT_FREEOWNED(self->variadicType.types.ptr);
		break;
	case OCT_TYPE_STRUCT:
		OCT_FREEOWNED(self->structType.fields.ptr);
		break;
	case OCT_TYPE_ARRAY:
		break;
	case OCT_TYPE_FIXED_SIZE_ARRAY:
		break;
	case OCT_TYPE_POINTER:
		break;
	case OCT_TYPE_PROTOCOL:
		OCT_FREEOWNED(self->protocolType.functions.ptr);
		break;
	}
	return oct_True;
}

