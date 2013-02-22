#include "oct_string.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_u8array.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Object
static oct_VTable StringObjectVTable;
static oct_BVTable BStringObjectVTable;

// EqComparable
static oct_EqComparableVTable StringEqComparableVTable;
static oct_BVTable BStringEqComparableVTable;

// Hashable
static oct_HashableVTable StringHashableVTable;
static oct_BVTable BStringHashableVTable;

// HashtableKey
static oct_HashtableKeyVTable StringHashtableKeyVTable;
static oct_BVTable BStringHashtableKeyVTable;

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_String_init(struct oct_Context* ctx) {
	oct_BType t;

	// String
	t = ctx->rt->builtInTypes.String;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_String);
	CHECK(oct_OAField_alloc(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_String, size);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_String, utf8Data);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OAU8;

	// String VTable for Object
	StringObjectVTable.objectType = t;
	BStringObjectVTable.ptr = &StringObjectVTable;
	CHECK(oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Object, t, BStringObjectVTable));

	// String VTable for EqComparable
	StringEqComparableVTable.type = t;
	StringEqComparableVTable.functions.equals = (oct_Bool(*)(oct_Context*,oct_BSelf,oct_BSelf,oct_Bool*))oct_BString_equals;
	BStringEqComparableVTable.ptr = (oct_VTable*)&StringEqComparableVTable;
	CHECK(oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.EqComparable, t, BStringEqComparableVTable));

	// String VTable for Hashable
	StringHashableVTable.type = t;
	StringHashableVTable.functions.hash = (oct_Bool(*)(oct_Context*,oct_BSelf,oct_Uword*))oct_String_hash;
	BStringHashableVTable.ptr = (oct_VTable*)&StringHashableVTable;
	CHECK(oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.Hashable, t, BStringHashableVTable));

	// String VTable for HashtableKey
	StringHashtableKeyVTable.type = t;
	StringHashtableKeyVTable.functions.eq = StringEqComparableVTable.functions;
	StringHashtableKeyVTable.functions.hashable = StringHashableVTable.functions;
	BStringHashtableKeyVTable.ptr = (oct_VTable*)&StringHashtableKeyVTable;
	CHECK(oct_Protocol_addImplementation(ctx, ctx->rt->builtInProtocols.HashtableKey, t, BStringHashtableKeyVTable));

	// OString
	t = ctx->rt->builtInTypes.OString;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.String;

	// BString
	t = ctx->rt->builtInTypes.BString;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.String;

	return oct_True;
}

oct_Bool oct_String_ctorCharArray(struct oct_Context* ctx, oct_String* str, oct_OAChar chars, oct_Uword idx, oct_Uword len) {
	oct_Uword si;
	oct_Uword ci;
	
	if(idx >= chars.ptr->size || (idx + len) >= chars.ptr->size) {
		oct_Context_setErrorWithCMessage(ctx, "Index out of bounds");
		return oct_False;
	}
	
	str->size = len;
	if(!oct_OAU8_alloc(ctx, len + 1, &str->utf8Data)) {
		return oct_False;
	}

	si = 0;
	ci = idx;
	for(; si < len; ++si, ++ci) {
		// TODO: convert to UTF-8 for real
		str->utf8Data.ptr->data[si] = chars.ptr->data[ci];
	}
	str->utf8Data.ptr->data[str->size] = '\0';
	return oct_True;
}

oct_Bool oct_OString_createFromCharArray(struct oct_Context* ctx, oct_OAChar chars, oct_Uword idx, oct_Uword len, oct_OString* out_str) {
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_String), (void**)&out_str->ptr)) {
        return oct_False;
    }
	if(!oct_String_ctorCharArray(ctx, out_str->ptr, chars, idx, len)) {
        oct_ExchangeHeap_freeRaw(ctx, out_str->ptr);
		out_str->ptr = NULL;
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_String_createOwnedFromCString(struct oct_Context* ctx, const char* cstr, oct_OString* out_str) {
	return oct_String_createOwnedFromCStringLen(ctx, cstr, strlen(cstr), out_str);
}

oct_Bool oct_String_createOwnedFromCStringLen(struct oct_Context* ctx, const char* cstr, oct_Uword strLen, oct_OString* out_str) {
	oct_Bool result;
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_String), (void**)&out_str->ptr)) {
        return oct_False;
    }
	out_str->ptr->size = strLen; // TODO: proper size in unicode code points
	result = oct_OAU8_alloc(ctx, strLen + 1, &out_str->ptr->utf8Data);
	if(!result) {
        oct_ExchangeHeap_freeRaw(ctx, out_str->ptr);
		return oct_False;
	}
	memcpy(&out_str->ptr->utf8Data.ptr->data[0], cstr, strLen + 1);
	return oct_True;
}

oct_Bool oct_String_destroyOwned(struct oct_Context* ctx, oct_OString str) {
	oct_Bool result = oct_String_dtor(ctx, str.ptr);
    return oct_ExchangeHeap_freeRaw(ctx, str.ptr) && result;
}

oct_Bool oct_String_ctorCStringLen(struct oct_Context* ctx, oct_String* str, const char* cstr, oct_Uword strlen) {
	oct_Bool result;
	oct_OAU8 u8data;
	result = oct_OAU8_alloc(ctx, strlen + 1, &u8data);
	if(!result) {
		return result;
	}
	memcpy(&u8data.ptr->data[0], cstr, strlen + 1);
	return oct_String_ctor(ctx, str, u8data);
}

oct_Bool oct_String_ctor(struct oct_Context* ctx, oct_String* str, oct_OAU8 utf8Data) {
	str->size = utf8Data.ptr->size - 1; // TODO: proper size in unicode codepoints
	str->utf8Data = utf8Data;
	return oct_True;
}

oct_Bool oct_String_dtor(struct oct_Context* ctx, oct_String* str) {
    // TODO: proper free/dtor here
	free(str->utf8Data.ptr);
	return oct_True;
}

oct_Bool oct_BString_equals(struct oct_Context* ctx, oct_BString x, oct_BString y, oct_Bool* out_result) {
	if(x.ptr->size != y.ptr->size) {
		*out_result = oct_False;
		return oct_True;
	}
	// TODO: UTF-8
	// This comparison has to be changed because the backing array may be a different size from the
	// string size since the string size is supposed to be in logical characters, not bytes.
	*out_result = (memcmp(&x.ptr->utf8Data.ptr->data[0], &y.ptr->utf8Data.ptr->data[0], x.ptr->size) == 0);
	return oct_True;
}

oct_Bool oct_BStringCString_equals(struct oct_Context* ctx, oct_BString str, const char* cstr, oct_Bool* out_result) {
	oct_Uword len = strlen(cstr);
	if(str.ptr->size != len) {
		*out_result = oct_False;
		return oct_True;
	}
	// TODO: UTF-8
	// This comparison has to be changed because the backing array may be a different size from the
	// string size since the string size is supposed to be in logical characters, not bytes.
	*out_result = (memcmp(&str.ptr->utf8Data.ptr->data[0], cstr, str.ptr->size) == 0);
	return oct_True;
}

	//oct_BType instanceType;
	//oct_Bool(*ctor)       (struct oct_Context* ctx, void* object, oct_OList args);
	//oct_Bool(*dtor)       (struct oct_Context* ctx, void* object);
	////oct_Bool(*print)      (struct oct_Context* ctx, void* object, /*Text output stream*/);
	//oct_Bool(*invoke)     (struct oct_Context* ctx, void* object, oct_OList args);
	////oct_Bool(*eval)       (struct oct_Context* ctx, void* object);
	//oct_Bool(*copyOwned)  (struct oct_Context* ctx, void* object, void** out_copy);
	//oct_Bool(*copyManaged)(struct oct_Context* ctx, void* object, void** out_copy);
	//oct_Bool(*hash)       (struct oct_Context* ctx, void* object, oct_Uword* out_hash);
	//oct_Bool(*equals)     (struct oct_Context* ctx, void* object, oct_BObject other, oct_Bool* out_result);

//static oct_ObjectVTable StringAsObject = {
//	
//};

oct_Bool oct_String_asObject(struct oct_Context* ctx, oct_OString str, oct_OObject* out_object) {
	out_object->self.self = str.ptr;
	out_object->vtable = (oct_ObjectVTable*)&StringObjectVTable;
	return oct_True;
}










