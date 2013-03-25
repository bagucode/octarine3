#include "oct_u8array.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <memory.h>

// Private

oct_Bool _oct_AU8_init(struct oct_Context* ctx) {

	// AU8
	ctx->rt->builtInTypes.AU8.ptr->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.AU8.ptr->arrayType.elementType = ctx->rt->builtInTypes.U8;

	// OAU8
	ctx->rt->builtInTypes.OAU8.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OAU8.ptr->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OAU8.ptr->pointerType.type = ctx->rt->builtInTypes.AU8;

	// BAU8
	ctx->rt->builtInTypes.BAU8.ptr->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BAU8.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BAU8.ptr->pointerType.type = ctx->rt->builtInTypes.AU8;

	return oct_True;
}

// Public

oct_Bool oct_AU8_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAU8* out_result) {
	oct_Uword allocSize = sizeof(oct_AU8) + (sizeof(oct_U8) * size);
    if(!OCT_ALLOCOWNED(allocSize, (void**)&out_result->ptr, "oct_AU8_createOwned")) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// "construct"
	memset(&out_result->ptr->data[0], 0, (sizeof(oct_U8) * size));
	return oct_True;
}

// FNV1a hash algorithm

#ifdef OCT64
#define FNV_OFFSET_BASIS 14695981039346656037ul
#define FNV_PRIME 1099511628211
#else
#define FNV_OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619
#endif

static oct_Uword fnv1a(void* data, oct_Uword length) {
	oct_Uword hash = FNV_OFFSET_BASIS;
	char* p = (char*)data;
	char* end = p + length;
	while(p < end) {
		hash ^= (oct_Uword)*p++;
		hash *= FNV_PRIME;
	}
	return hash;
}

oct_Bool oct_AU8_hash(struct oct_Context* ctx, oct_BAU8 self, oct_Uword* out_hash) {
	*out_hash = fnv1a(self.ptr->data, self.ptr->size);
	return oct_True;
}

oct_Bool oct_AU8_copyOwned(struct oct_Context* ctx, oct_BSelf orig, oct_OSelf* out_copy) {
	oct_BAU8 bau8;
	oct_Uword allocSize;
	bau8.ptr = (oct_AU8*)orig.self;
	allocSize = sizeof(oct_AU8) + (sizeof(oct_U8) * bau8.ptr->size);
    if(!OCT_ALLOCOWNED(allocSize, (void**)&out_copy->self, "oct_AU8_copyOwned")) {
        return oct_False;
    }
	memcpy(out_copy->self, orig.self, allocSize);
	return oct_True;
}
