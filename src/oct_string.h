#ifndef oct_string
#define oct_string

#include "oct_u8array.h"
#include "oct_chararray.h"

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

oct_Bool _oct_String_initType(struct oct_Context* ctx);
oct_Bool _oct_OString_initType(struct oct_Context* ctx);
oct_Bool _oct_BString_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_String_ctor(struct oct_Context* ctx, oct_String* str, oct_OAU8 utf8Data);
oct_Bool oct_String_ctorCharArray(struct oct_Context* ctx, oct_String* str, oct_OAChar chars, oct_Uword idx, oct_Uword len);
oct_Bool oct_String_ctorCStringLen(struct oct_Context* ctx, oct_String* str, const char* cstr, oct_Uword strlen);
oct_Bool oct_String_dtor(struct oct_Context* ctx, oct_String* str);

oct_Bool oct_OString_createFromCString(struct oct_Context* ctx, const char* cstr, oct_OString* out_str);
oct_Bool oct_OString_createFromCStringLen(struct oct_Context* ctx, const char* cstr, oct_Uword strLen, oct_OString* out_str);
oct_Bool oct_OString_createFromCharArray(struct oct_Context* ctx, oct_OAChar chars, oct_Uword idx, oct_Uword len, oct_OString* out_str);
oct_Bool oct_OString_destroy(struct oct_Context* ctx, oct_OString str);

#endif
