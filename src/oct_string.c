#include "oct_string.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_u8array.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Private

oct_Bool _oct_String_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.String;
	oct_Bool result;

	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_String);
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_String, size);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Uword;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_String, utf8Data);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.OAU8;
	return oct_True;
}

oct_Bool _oct_OString_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OString->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OString->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OString->pointerType.type.ptr = ctx->rt->builtInTypes.String;
	return oct_True;
}

// Public

oct_Bool oct_String_ctorCharArray(struct oct_Context* ctx, oct_String* str, oct_OAChar chars, oct_Uword idx, oct_Uword len) {
	oct_Uword si;
	oct_Uword ci;
	
	if(idx >= chars.ptr->size || (idx + len) >= chars.ptr->size) {
		// TODO: out of bounds error
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
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_String), (void**)&out_str->ptr)) {
        return oct_False;
    }
	if(!oct_String_ctorCharArray(ctx, out_str->ptr, chars, idx, len)) {
        oct_ExchangeHeap_free(ctx, out_str->ptr);
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
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_String), (void**)&out_str->ptr)) {
        return oct_False;
    }
	out_str->ptr->size = strLen; // TODO: proper size in unicode code points
	result = oct_OAU8_alloc(ctx, strLen + 1, &out_str->ptr->utf8Data);
	if(!result) {
        oct_ExchangeHeap_free(ctx, out_str->ptr);
		return oct_False;
	}
	memcpy(&out_str->ptr->utf8Data.ptr->data[0], cstr, strLen + 1);
	return oct_True;
}

oct_Bool oct_String_destroyOwned(struct oct_Context* ctx, oct_OString str) {
	oct_Bool result = oct_String_dtor(ctx, str.ptr);
    return oct_ExchangeHeap_free(ctx, str.ptr) && result;
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
