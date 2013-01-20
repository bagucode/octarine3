#ifndef oct_primitives
#define oct_primitives

#ifdef _WIN32

typedef __int8           oct_I8;
typedef unsigned __int8  oct_U8;
typedef __int16          oct_I16;
typedef unsigned __int16 oct_U16;
typedef __int32          oct_I32;
typedef unsigned __int32 oct_U32;
typedef __int64          oct_I64;
typedef unsigned __int64 oct_U64;
typedef float            oct_F32;
typedef double           oct_F64;

typedef oct_U8 oct_Bool;
#define oct_True 1
#define oct_False 0

typedef oct_I32 oct_Char;

#ifdef _WIN64
#define OCT64
typedef oct_I64 oct_Word;
typedef oct_U64 oct_Uword;
#else
typedef oct_I32 oct_Word;
typedef oct_U32 oct_Uword;
#endif

#else

#endif

// Private

struct oct_Context;

oct_Bool _oct_Primitives_initType(struct oct_Context* ctx);

// Public

#endif
