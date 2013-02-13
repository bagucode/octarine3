#ifndef oct_primitives_functions
#define oct_primitives_functions

#include "oct_primitives.h"
#include "oct_primitive_pointers.h"
#include "oct_object.h"

struct oct_Context;

oct_Bool oct_I8_asObject(struct oct_Context* ctx, oct_OI8 i8, oct_OObject* out_obj);
oct_Bool oct_U8_asObject(struct oct_Context* ctx, oct_OU8 u8, oct_OObject* out_obj);
oct_Bool oct_I16_asObject(struct oct_Context* ctx, oct_OI16 i16, oct_OObject* out_obj);
oct_Bool oct_U16_asObject(struct oct_Context* ctx, oct_OU16 u16, oct_OObject* out_obj);
oct_Bool oct_I32_asObject(struct oct_Context* ctx, oct_OI32 i32, oct_OObject* out_obj);
oct_Bool oct_U32_asObject(struct oct_Context* ctx, oct_OU32 u32, oct_OObject* out_obj);
oct_Bool oct_I64_asObject(struct oct_Context* ctx, oct_OI64 i64, oct_OObject* out_obj);
oct_Bool oct_U64_asObject(struct oct_Context* ctx, oct_OU64 u64, oct_OObject* out_obj);
oct_Bool oct_F32_asObject(struct oct_Context* ctx, oct_OF32 f32, oct_OObject* out_obj);
oct_Bool oct_F64_asObject(struct oct_Context* ctx, oct_OF64 f64, oct_OObject* out_obj);
oct_Bool oct_Bool_asObject(struct oct_Context* ctx, oct_OBool b, oct_OObject* out_obj);
oct_Bool oct_Char_asObject(struct oct_Context* ctx, oct_OChar c, oct_OObject* out_obj);
oct_Bool oct_Word_asObject(struct oct_Context* ctx, oct_OWord word, oct_OObject* out_obj);
oct_Bool oct_Uword_asObject(struct oct_Context* ctx, oct_OUword uword, oct_OObject* out_obj);

#endif
