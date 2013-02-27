#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"
#include "oct_exchangeheap.h"
#include "oct_type.h"
#include "oct_function.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Object_init(struct oct_Context* ctx) {

	// Object protocol. No functions.
	oct_BType t = ctx->rt->builtInTypes.Object;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 0, &t.ptr->protocolType.functions));

	// OObject
	t = ctx->rt->builtInTypes.OObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// BObject
	t = ctx->rt->builtInTypes.BObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// OObjectOption
	t = ctx->rt->builtInTypes.OObjectOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OObjectOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OObject;

	return oct_True;
}

// The output is BObject because C does not have templates but the output should be safe to manually cast to the given protocol
oct_Bool oct_Object_as(oct_Context* ctx, oct_BSelf object, oct_BType selfType, oct_BProtocolBinding protocol, oct_BObject* out_casted) {
	oct_BHashtable table;
	oct_BHashtableKey key;
	oct_BObject vtableObject;

	table.ptr = &protocol.ptr->implementations;
	CHECK(oct_BType_asHashtableKey(ctx, selfType, &key));
	// TODO: change hashtable to return OObjectOption/BObjectOption instead of just OObject/BObject
	CHECK(oct_Hashtable_borrow(ctx, table, key, &vtableObject));
	out_casted->self = object;
	out_casted->vtable = (oct_ObjectVTable*)vtableObject.self.self;
	return oct_True;
}

// The output is untyped because C does not have templates but the output should be safe to manually
// cast to the given protocol
oct_Bool oct_Object_as(oct_Context* ctx, oct_BSelf object, oct_BType type, oct_BType protocol, oct_BObject* out_casted);

oct_Bool oct_Object_destroyOwned(oct_Context* ctx, oct_OObject obj);


//oct_Bool _oct_Object_initType(struct oct_Context* ctx) {
//}
//
//oct_Bool oct_Object_symbolp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
//	*out_bool = ctx->rt->builtInTypes.Symbol == obj.object.vtable->instanceType.ptr;
//	return oct_True;
//}
//
//oct_Bool oct_Object_stringp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
//	*out_bool = ctx->rt->builtInTypes.String == obj.object.vtable->instanceType.ptr;
//	return oct_True;
//}
//
//oct_Bool oct_Object_listp(struct oct_Context* ctx, oct_BObject obj, oct_Bool* out_bool) {
//	*out_bool = ctx->rt->builtInTypes.List == obj.object.vtable->instanceType.ptr;
//	return oct_True;
//}
//
////oct_Bool oct_Object_ctor(struct oct_Context* ctx, oct_Object obj, oct_OList args) {
////	return obj.vtable->ctor(ctx, obj.object, args);
////}
//
//oct_Bool oct_Object_dtor(struct oct_Context* ctx, oct_Object obj) {
//	return obj.vtable->dtor(ctx, obj.object);
//}
//
////oct_Bool oct_Object_print(struct oct_Context* ctx, oct_Object obj, /*Text output stream*/);
//
//oct_Bool oct_Object_invoke(struct oct_Context* ctx, oct_Object obj, oct_OList args) {
//	return obj.vtable->invoke(ctx, obj.object, args);
//}
//
////oct_Bool oct_Object_eval(struct oct_Context* ctx, oct_Object obj);
//
//oct_Bool oct_Object_copyOwned(struct oct_Context* ctx, oct_Object obj, void** out_copy) {
//	return obj.vtable->copyOwned(ctx, obj.object, out_copy);
//}
//
//oct_Bool oct_Object_copyManaged(struct oct_Context* ctx, oct_Object obj, void** out_copy) {
//	return obj.vtable->copyManaged(ctx, obj.object, out_copy);
//}
//
//oct_Bool oct_Object_hash(struct oct_Context* ctx, oct_Object obj, oct_Uword* out_hash) {
//	return obj.vtable->hash(ctx, obj.object, out_hash);
//}
//
//oct_Bool oct_Object_equals(struct oct_Context* ctx, oct_Object obj, oct_BObject other, oct_Bool* out_result) {
//	return obj.vtable->equals(ctx, obj.object, other, out_result);
//}
//
//oct_Bool oct_Object_destroyOwned(struct oct_Context* ctx, oct_OObject obj) {
//	oct_Bool result = oct_Object_dtor(ctx, obj.object);
//	return oct_ExchangeHeap_freeRaw(ctx, obj.object.object) && result;
//}
//
//typedef struct Array {
//	oct_Uword size;
//	oct_U8 data[];
//} Array;
//
//oct_Bool oct_Object_sizeOf(struct oct_Context* ctx, oct_BObject obj, oct_Uword* out_size) {
//	*out_size = oct_Type_sizeOf(ctx, obj.object.vtable->instanceType, out_size);
//	if(obj.object.vtable->instanceType.ptr->variant == OCT_TYPE_ARRAY) {
//		*out_size = sizeof(oct_Uword) + ( ((Array*)obj.object.object)->size * (*out_size));
//	}
//	return oct_True;
//}
//
