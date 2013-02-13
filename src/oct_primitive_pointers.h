#ifndef oct_primitive_pointers
#define oct_primitive_pointers

#include "oct_primitives.h"

// I8

typedef struct oct_OI8 {
	oct_I8* ptr;
} oct_OI8;

typedef struct oct_MI8 {
	oct_I8* ptr;
} oct_MI8;

typedef struct oct_BI8 {
	oct_I8* ptr;
} oct_BI8;

// U8

typedef struct oct_OU8 {
	oct_U8* ptr;
} oct_OU8;

typedef struct oct_MU8 {
	oct_U8* ptr;
} oct_MU8;

typedef struct oct_BU8 {
	oct_U8* ptr;
} oct_BU8;

// I16

typedef struct oct_OI16 {
	oct_I16* ptr;
} oct_OI16;

typedef struct oct_MI16 {
	oct_I16* ptr;
} oct_MI16;

typedef struct oct_BI16 {
	oct_I16* ptr;
} oct_BI16;

// U16

typedef struct oct_OU16 {
	oct_U16* ptr;
} oct_OU16;

typedef struct oct_MU16 {
	oct_U16* ptr;
} oct_MU16;

typedef struct oct_BU16 {
	oct_U16* ptr;
} oct_BU16;

// I32

typedef struct oct_OI32 {
	oct_I32* ptr;
} oct_OI32;

typedef struct oct_MI32 {
	oct_I32* ptr;
} oct_MI32;

typedef struct oct_BI32 {
	oct_I32* ptr;
} oct_BI32;

// U32

typedef struct oct_OU32 {
	oct_U32* ptr;
} oct_OU32;

typedef struct oct_MU32 {
	oct_U32* ptr;
} oct_MU32;

typedef struct oct_BU32 {
	oct_U32* ptr;
} oct_BU32;

// I64

typedef struct oct_OI64 {
	oct_I64* ptr;
} oct_OI64;

typedef struct oct_MI64 {
	oct_I64* ptr;
} oct_MI64;

typedef struct oct_BI64 {
	oct_I64* ptr;
} oct_BI64;

// U64

typedef struct oct_OU64 {
	oct_U64* ptr;
} oct_OU64;

typedef struct oct_MU64 {
	oct_U64* ptr;
} oct_MU64;

typedef struct oct_BU64 {
	oct_U64* ptr;
} oct_BU64;

// F32

typedef struct oct_OF32 {
	oct_F32* ptr;
} oct_OF32;

typedef struct oct_MF32 {
	oct_F32* ptr;
} oct_MF32;

typedef struct oct_BF32 {
	oct_F32* ptr;
} oct_BF32;

// F64

typedef struct oct_OF64 {
	oct_F64* ptr;
} oct_OF64;

typedef struct oct_MF64 {
	oct_F64* ptr;
} oct_MF64;

typedef struct oct_BF64 {
	oct_F64* ptr;
} oct_BF64;

// Bool

typedef struct oct_OBool {
	oct_Bool* ptr;
} oct_OBool;

typedef struct oct_MBool {
	oct_Bool* ptr;
} oct_MBool;

typedef struct oct_BBool {
	oct_Bool* ptr;
} oct_BBool;

// Char

typedef struct oct_OChar {
	oct_Char* ptr;
} oct_OChar;

typedef struct oct_MChar {
	oct_Char* ptr;
} oct_MChar;

typedef struct oct_BChar {
	oct_Char* ptr;
} oct_BChar;

// Word

typedef struct oct_OWord {
	oct_Word* ptr;
} oct_OWord;

typedef struct oct_MWord {
	oct_Word* ptr;
} oct_MWord;

typedef struct oct_BWord {
	oct_Word* ptr;
} oct_BWord;

// Uword

typedef struct oct_OUword {
	oct_Uword* ptr;
} oct_OUword;

typedef struct oct_MUword {
	oct_Uword* ptr;
} oct_MUword;

typedef struct oct_BUword {
	oct_Uword* ptr;
} oct_BUword;

#endif
