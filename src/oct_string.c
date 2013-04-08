#include "oct_string.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_u8array.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_String_init(struct oct_Context* ctx) {
	oct_CType t;

	// String
	t = ctx->rt->builtInTypes.String;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_String);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_String, size);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.Uword;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_String, utf8Data);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OAU8;

	// String VTable for Object {dtor}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.StringAsObject, t, oct_String_dtor));

	// String VTable for EqComparable {eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.EqComparable, 1, &ctx->rt->vtables.StringAsEqComparable, t, oct_BString_equals));

	// String VTable for Hashable {hash}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Hashable, 1, &ctx->rt->vtables.StringAsHashable, t, oct_String_hash));

	// String VTable for HashtableKey {hash, eq}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.HashtableKey, 2, &ctx->rt->vtables.StringAsHashtableKey, t, oct_String_hash, oct_BString_equals));

	// String VTable for Copyable {copyOwned}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Copyable, 1, &ctx->rt->vtables.StringAsCopyable, t, oct_String_copyOwned));

	// String VTable for Printable {print}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Printable, 1, &ctx->rt->vtables.StringAsPrintable, t, oct_String_print));

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
	if(!oct_AU8_createOwned(ctx, len + 1, &str->utf8Data)) {
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

oct_Bool oct_String_createOwnedFromCharArray(struct oct_Context* ctx, oct_OAChar chars, oct_Uword idx, oct_Uword len, oct_OString* out_str) {
    if(!OCT_ALLOCOWNED(sizeof(oct_String), (void**)&out_str->ptr, "oct_String_createOwnedFromCharArray")) {
        return oct_False;
    }
	if(!oct_String_ctorCharArray(ctx, out_str->ptr, chars, idx, len)) {
        OCT_FREEOWNED(out_str->ptr);
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
    if(!OCT_ALLOCOWNED(sizeof(oct_String), (void**)&out_str->ptr, "oct_String_createOwnedFromCStringLen")) {
        return oct_False;
    }
	out_str->ptr->size = strLen; // TODO: proper size in unicode code points
	result = oct_AU8_createOwned(ctx, strLen + 1, &out_str->ptr->utf8Data);
	if(!result) {
        OCT_FREEOWNED(out_str->ptr);
		return oct_False;
	}
	memcpy(&out_str->ptr->utf8Data.ptr->data[0], cstr, strLen + 1);
	return oct_True;
}

oct_Bool oct_String_destroyOwned(struct oct_Context* ctx, oct_OString str) {
	oct_BSelf self;
	oct_Bool result;
	self.self = str.ptr;
	result = oct_String_dtor(ctx, self);
    return OCT_FREEOWNED(str.ptr) && result;
}

oct_Bool oct_String_ctorCStringLen(struct oct_Context* ctx, oct_String* str, const char* cstr, oct_Uword strlen) {
	oct_Bool result;
	oct_OAU8 u8data;
	result = oct_AU8_createOwned(ctx, strlen + 1, &u8data);
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

oct_Bool oct_String_dtor(struct oct_Context* ctx, oct_BSelf str) {
    // TODO: proper free/dtor here
	OCT_FREEOWNED(((oct_String*)str.self)->utf8Data.ptr);
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

oct_Bool oct_String_hash(struct oct_Context* ctx, oct_BString str, oct_Uword* out_hash) {
	oct_BAU8 arr;
	arr.ptr = str.ptr->utf8Data.ptr;
	return oct_AU8_hash(ctx, arr, out_hash);
}

oct_Bool oct_String_asObjectOwned(struct oct_Context* ctx, oct_OString str, oct_OObject* out_object) {
	out_object->self.self = str.ptr;
	out_object->vtable = (oct_ObjectVTable*)ctx->rt->vtables.StringAsObject.ptr;
	return oct_True;
}

//oct_Bool oct_String_asHashableOwned(struct oct_Context* ctx, oct_OString str, oct_OHashable* out_hashable) {
//	out_hashable->self.self = str.ptr;
//	out_hashable->vtable = (oct_HashableVTable*)ctx->rt->vtables.StringAsHashable.ptr;
//	return oct_True;
//}
//
//oct_Bool oct_String_asEqComparableOwned(struct oct_Context* ctx, oct_OString str, oct_OEqComparable* out_eq) {
//	out_eq->self.self = str.ptr;
//	out_eq->vtable = (oct_EqComparableVTable*)ctx->rt->vtables.StringAsEqComparable.ptr;
//	return oct_True;
//}

oct_Bool oct_String_asHashtableKeyOwned(struct oct_Context* ctx, oct_OString str, oct_OHashtableKey* out_key) {
	out_key->self.self = str.ptr;
	out_key->vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.StringAsHashtableKey.ptr;
	return oct_True;
}

oct_Bool oct_String_asHashtableKeyBorrowed(struct oct_Context* ctx, oct_BString str, oct_BHashtableKey* out_key) {
	out_key->self.self = str.ptr;
	out_key->vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.StringAsHashtableKey.ptr;
	return oct_True;
}

oct_Bool oct_String_asCopyable(struct oct_Context* ctx, oct_BString str, oct_BCopyable* out_key) {
	out_key->self.self = str.ptr;
	out_key->vtable = (oct_CopyableVTable*)ctx->rt->vtables.StringAsCopyable.ptr;
	return oct_True;
}

oct_Bool oct_String_copyOwned(struct oct_Context* ctx, oct_BSelf orig, oct_OSelf* out_cpy) {
	oct_String* org;
	oct_String* cpy;
    if(!OCT_ALLOCOWNED(sizeof(oct_String), &out_cpy->self, "oct_String_copyOwned")) {
        return oct_False;
    }
	org = (oct_String*)orig.self;
	cpy = (oct_String*)out_cpy->self;
	cpy->size = org->size;
	orig.self = org->utf8Data.ptr;
	return oct_AU8_copyOwned(ctx, orig, (oct_OSelf*)&cpy->utf8Data);
}

oct_Bool oct_String_print(struct oct_Context* ctx, oct_BString str) {
	printf("\"%s\"", str.ptr->utf8Data.ptr->data);
	return oct_True;
}

oct_Bool oct_String_asPrintable(struct oct_Context* ctx, oct_BString str, oct_BPrintable* out_prn) {
	out_prn->self.self = str.ptr;
	out_prn->vtable = (oct_PrintableVTable*)ctx->rt->vtables.StringAsPrintable.ptr;
	return oct_True;
}
