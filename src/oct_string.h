#ifndef oct_string
#define oct_string

#include "oct_u8array.h"
#include "oct_chararray.h"
#include "oct_object.h"
#include "oct_hashtable.h"
#include "oct_self.h"
#include "oct_copyable.h"

typedef struct oct_String {
	oct_Uword size;
	oct_OAU8 utf8Data;
} oct_String;

// Owned string
typedef struct oct_OString {
	oct_String* ptr;
} oct_OString;

typedef struct oct_BString {
	oct_String* ptr;
} oct_BString;

// Private

oct_Bool _oct_String_init(struct oct_Context* ctx);

// Public

oct_Bool oct_String_ctor(struct oct_Context* ctx, oct_String* str, oct_OAU8 utf8Data);
oct_Bool oct_String_ctorCharArray(struct oct_Context* ctx, oct_String* str, oct_OAChar chars, oct_Uword idx, oct_Uword len);
oct_Bool oct_String_ctorCString(struct oct_Context* ctx, oct_String* str, const char* cstr);
oct_Bool oct_String_ctorCStringLen(struct oct_Context* ctx, oct_String* str, const char* cstr, oct_Uword strlen);
oct_Bool oct_String_dtor(struct oct_Context* ctx, oct_BSelf str);

oct_Bool oct_String_createOwnedFromCString(struct oct_Context* ctx, const char* cstr, oct_OString* out_str);
oct_Bool oct_String_createOwnedFromCStringLen(struct oct_Context* ctx, const char* cstr, oct_Uword strLen, oct_OString* out_str);
oct_Bool oct_String_createOwnedFromCharArray(struct oct_Context* ctx, oct_OAChar chars, oct_Uword idx, oct_Uword len, oct_OString* out_str);
oct_Bool oct_String_destroyOwned(struct oct_Context* ctx, oct_OString str);
oct_Bool oct_String_copyOwned(struct oct_Context* ctx, oct_BSelf orig, oct_OSelf* out_cpy);

oct_Bool oct_BString_equals(struct oct_Context* ctx, oct_BString x, oct_BString y, oct_Bool* out_result);
oct_Bool oct_BStringCString_equals(struct oct_Context* ctx, oct_BString str, const char* cstr, oct_Bool* out_result);

oct_Bool oct_String_hash(struct oct_Context* ctx, oct_BString str, oct_Uword* out_hash);

// Protocol casts
oct_Bool oct_String_asObjectOwned(struct oct_Context* ctx, oct_OString str, oct_OObject* out_object);
//oct_Bool oct_String_asHashableOwned(struct oct_Context* ctx, oct_OString str, oct_OHashable* out_hashable);
//oct_Bool oct_String_asEqComparableOwned(struct oct_Context* ctx, oct_OString str, oct_OEqComparable* out_eq);
oct_Bool oct_String_asHashtableKeyOwned(struct oct_Context* ctx, oct_OString str, oct_OHashtableKey* out_key);
oct_Bool oct_String_asHashtableKeyBorrowed(struct oct_Context* ctx, oct_BString str, oct_BHashtableKey* out_key);
oct_Bool oct_String_asCopyable(struct oct_Context* ctx, oct_BString str, oct_BCopyable* out_key);

#endif
